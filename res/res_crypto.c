/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Provide Cryptographic Signature capability
 * 
 * Copyright (C) 1999, Mark Spencer
 *
 * Mark Spencer <markster@linux-support.net>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include <sys/types.h>
#include <asterisk/file.h>
#include <asterisk/channel.h>
#include <asterisk/logger.h>
#include <asterisk/say.h>
#include <asterisk/module.h>
#include <asterisk/options.h>
#include <asterisk/crypto.h>
#include <asterisk/md5.h>
#include <asterisk/cli.h>
#include <asterisk/io.h>
#include <asterisk/lock.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "../asterisk.h"
#include "../astconf.h"

/*
 * Asterisk uses RSA keys with SHA-1 message digests for its
 * digital signatures.  The choice of RSA is due to its higher
 * throughput on verification, and the choice of SHA-1 based
 * on the recently discovered collisions in MD5's compression 
 * algorithm and recommendations of avoiding MD5 in new schemes
 * from various industry experts.
 *
 * We use OpenSSL to provide our crypto routines, although we never
 * actually use full-up SSL
 *
 */

/*
 * XXX This module is not very thread-safe.  It is for everyday stuff
 *     like reading keys and stuff, but there are all kinds of weird
 *     races with people running reload and key init at the same time
 *     for example
 *
 * XXXX
 */

static char base64[64];
static char b2a[256];

AST_MUTEX_DEFINE_STATIC(keylock);

#define KEY_NEEDS_PASSCODE (1 << 16)

struct ast_key {
	/* Name of entity */
	char name[80];
	/* File name */
	char fn[256];
	/* Key type (AST_KEY_PUB or AST_KEY_PRIV, along with flags from above) */
	int ktype;
	/* RSA structure (if successfully loaded) */
	RSA *rsa;
	/* Whether we should be deleted */
	int delme;
	/* FD for input (or -1 if no input allowed, or -2 if we needed input) */
	int infd;
	/* FD for output */
	int outfd;
	/* Last MD5 Digest */
	unsigned char digest[16];
	struct ast_key *next;
};

static struct ast_key *keys = NULL;


#if 0
static int fdprint(int fd, char *s)
{
        return write(fd, s, strlen(s) + 1);
}
#endif
static int pw_cb(char *buf, int size, int rwflag, void *userdata)
{
	struct ast_key *key = (struct ast_key *)userdata;
	char prompt[256];
	int res;
	int tmp;
	if (key->infd > -1) {
		snprintf(prompt, sizeof(prompt), ">>>> passcode for %s key '%s': ",
			 key->ktype == AST_KEY_PRIVATE ? "PRIVATE" : "PUBLIC", key->name);
		write(key->outfd, prompt, strlen(prompt));
		memset(buf, 0, sizeof(buf));
		tmp = ast_hide_password(key->infd);
		memset(buf, 0, size);
		res = read(key->infd, buf, size);
		ast_restore_tty(key->infd, tmp);
		if (buf[strlen(buf) -1] == '\n')
			buf[strlen(buf) - 1] = '\0';
		return strlen(buf);
	} else {
		/* Note that we were at least called */
		key->infd = -2;
	}
	return -1;
}

struct ast_key *ast_key_get(char *kname, int ktype)
{
	struct ast_key *key;
	ast_mutex_lock(&keylock);
	key = keys;
	while(key) {
		if (!strcmp(kname, key->name) &&
		    (ktype == key->ktype))
			break;
		key = key->next;
	}
	ast_mutex_unlock(&keylock);
	return key;
}

static struct ast_key *try_load_key (char *dir, char *fname, int ifd, int ofd, int *not2)
{
	int ktype = 0;
	char *c = NULL;
	char ffname[256];
	char digest[16];
	FILE *f;
	struct MD5Context md5;
	struct ast_key *key;
	static int notice = 0;
	int found = 0;

	/* Make sure its name is a public or private key */

	if ((c = strstr(fname, ".pub")) && !strcmp(c, ".pub")) {
		ktype = AST_KEY_PUBLIC;
	} else if ((c = strstr(fname, ".key")) && !strcmp(c, ".key")) {
		ktype = AST_KEY_PRIVATE;
	} else
		return NULL;

	/* Get actual filename */
	snprintf(ffname, sizeof(ffname), "%s/%s", dir, fname);

	ast_mutex_lock(&keylock);
	key = keys;
	while(key) {
		/* Look for an existing version already */
		if (!strcasecmp(key->fn, ffname)) 
			break;
		key = key->next;
	}
	ast_mutex_unlock(&keylock);

	/* Open file */
	f = fopen(ffname, "r");
	if (!f) {
		ast_log(LOG_WARNING, "Unable to open key file %s: %s\n", ffname, strerror(errno));
		return NULL;
	}
	MD5Init(&md5);
	while(!feof(f)) {
		/* Calculate a "whatever" quality md5sum of the key */
		char buf[256];
		fgets(buf, sizeof(buf), f);
		if (!feof(f)) {
			MD5Update(&md5, buf, strlen(buf));
		}
	}
	MD5Final(digest, &md5);
	if (key) {
		/* If the MD5 sum is the same, and it isn't awaiting a passcode 
		   then this is far enough */
		if (!memcmp(digest, key->digest, 16) &&
		    !(key->ktype & KEY_NEEDS_PASSCODE)) {
			fclose(f);
			key->delme = 0;
			return NULL;
		} else {
			/* Preserve keytype */
			ktype = key->ktype;
			/* Recycle the same structure */
			found++;
		}
	}

	/* Make fname just be the normal name now */
	*c = '\0';
	if (!key) {
		key = (struct ast_key *)malloc(sizeof(struct ast_key));
		if (!key) {
			ast_log(LOG_WARNING, "Out of memory\n");
			fclose(f);
			return NULL;
		}
		memset(key, 0, sizeof(struct ast_key));
	}
	/* At this point we have a key structure (old or new).  Time to
	   fill it with what we know */
	/* Gotta lock if this one already exists */
	if (found)
		ast_mutex_lock(&keylock);
	/* First the filename */
	strncpy(key->fn, ffname, sizeof(key->fn));
	/* Then the name */
	strncpy(key->name, fname, sizeof(key->name));
	key->ktype = ktype;
	/* Yes, assume we're going to be deleted */
	key->delme = 1;
	/* Keep the key type */
	memcpy(key->digest, digest, 16);
	/* Can I/O takes the FD we're given */
	key->infd = ifd;
	key->outfd = ofd;
	/* Reset the file back to the beginning */
	rewind(f);
	/* Now load the key with the right method */
	if (ktype == AST_KEY_PUBLIC)
		key->rsa = PEM_read_RSA_PUBKEY(f, NULL, pw_cb, key);
	else
		key->rsa = PEM_read_RSAPrivateKey(f, NULL, pw_cb, key);
	fclose(f);
	if (key->rsa) {
		/* Key loaded okay */
		key->ktype &= ~KEY_NEEDS_PASSCODE;
		if (option_verbose > 2)
			ast_verbose(VERBOSE_PREFIX_3 "Loaded %s key '%s'\n", key->ktype == AST_KEY_PUBLIC ? "PUBLIC" : "PRIVATE", key->name);
		if (option_debug)
			ast_log(LOG_DEBUG, "Key '%s' loaded OK\n", key->name);
		key->delme = 0;
	} else if (key->infd != -2) {
		ast_log(LOG_WARNING, "Key load %s '%s' failed\n",key->ktype == AST_KEY_PUBLIC ? "PUBLIC" : "PRIVATE", key->name);
		if (ofd > -1) {
			ERR_print_errors_fp(stderr);
		} else
			ERR_print_errors_fp(stderr);
	} else {
		ast_log(LOG_NOTICE, "Key '%s' needs passcode.\n", key->name);
		key->ktype |= KEY_NEEDS_PASSCODE;
		if (!notice) {
			if (!option_initcrypto) 
				ast_log(LOG_NOTICE, "Add the '-i' flag to the asterisk command line if you want to automatically initialize passcodes at launch.\n");
			notice++;
		}
		/* Keep it anyway */
		key->delme = 0;
		/* Print final notice about "init keys" when done */
		*not2 = 1;
	}
	if (found)
		ast_mutex_unlock(&keylock);
	if (!found) {
		ast_mutex_lock(&keylock);
		key->next = keys;
		keys = key;
		ast_mutex_unlock(&keylock);
	}
	return key;
}

#if 0

static void dump(unsigned char *src, int len)
{
	int x; 
	for (x=0;x<len;x++)
		printf("%02x", *(src++));
	printf("\n");
}

static char *binary(int y, int len)
{
	static char res[80];
	int x;
	memset(res, 0, sizeof(res));
	for (x=0;x<len;x++) {
		if (y & (1 << x))
			res[(len - x - 1)] = '1';
		else
			res[(len - x - 1)] = '0';
	}
	return res;
}

#endif

static int base64decode(unsigned char *dst, char *src, int max)
{
	int cnt = 0;
	unsigned int byte = 0;
	unsigned int bits = 0;
	int incnt = 0;
#if 0
	unsigned char *odst = dst;
#endif
	while(*src && (cnt < max)) {
		/* Shift in 6 bits of input */
		byte <<= 6;
		byte |= (b2a[(int)(*src)]) & 0x3f;
		bits += 6;
#if 0
		printf("Add: %c %s\n", *src, binary(b2a[(int)(*src)] & 0x3f, 6));
#endif
		src++;
		incnt++;
		/* If we have at least 8 bits left over, take that character 
		   off the top */
		if (bits >= 8)  {
			bits -= 8;
			*dst = (byte >> bits) & 0xff;
#if 0
			printf("Remove: %02x %s\n", *dst, binary(*dst, 8));
#endif
			dst++;
			cnt++;
		}
	}
#if 0
	dump(odst, cnt);
#endif
	/* Dont worry about left over bits, they're extra anyway */
	return cnt;
}

static int base64encode(char *dst, unsigned char *src, int srclen, int max)
{
	int cnt = 0;
	unsigned int byte = 0;
	int bits = 0;
	int index;
	int cntin = 0;
#if 0
	char *odst = dst;
	dump(src, srclen);
#endif
	/* Reserve one bit for end */
	max--;
	while((cntin < srclen) && (cnt < max)) {
		byte <<= 8;
#if 0
		printf("Add: %02x %s\n", *src, binary(*src, 8));
#endif
		byte |= *(src++);
		bits += 8;
		cntin++;
		while((bits >= 6) && (cnt < max)) {
			bits -= 6;
			/* We want only the top */
			index = (byte >> bits) & 0x3f;
			*dst = base64[index];
#if 0
			printf("Remove: %c %s\n", *dst, binary(index, 6));
#endif
			dst++;
			cnt++;
		}
	}
	if (bits && (cnt < max)) {
		/* Add one last character for the remaining bits, 
		   padding the rest with 0 */
		byte <<= (6 - bits);
		index = (byte) & 0x3f;
		*(dst++) = base64[index];
		cnt++;
	}
	*dst = '\0';
	return cnt;
}

int ast_sign(struct ast_key *key, char *msg, char *sig)
{
	unsigned char digest[20];
	unsigned char dsig[128];
	int siglen = sizeof(dsig);
	int res;

	if (key->ktype != AST_KEY_PRIVATE) {
		ast_log(LOG_WARNING, "Cannot sign with a private key\n");
		return -1;
	}

	/* Calculate digest of message */
	SHA1((unsigned char *)msg, strlen(msg), digest);

	/* Verify signature */
	res = RSA_sign(NID_sha1, digest, sizeof(digest), dsig, &siglen, key->rsa);
	
	if (!res) {
		ast_log(LOG_WARNING, "RSA Signature (key %s) failed\n", key->name);
		return -1;
	}

	if (siglen != sizeof(dsig)) {
		ast_log(LOG_WARNING, "Unexpected signature length %d, expecting %d\n", (int)siglen, (int)sizeof(dsig));
		return -1;
	}

	/* Success -- encode (256 bytes max as documented) */
	base64encode(sig, dsig, siglen, 256);
	return 0;
	
}

int ast_check_signature(struct ast_key *key, char *msg, char *sig)
{
	unsigned char digest[20];
	unsigned char dsig[128];
	int res;

	if (key->ktype != AST_KEY_PUBLIC) {
		/* Okay, so of course you really *can* but for our purposes
		   we're going to say you can't */
		ast_log(LOG_WARNING, "Cannot check message signature with a private key\n");
		return -1;
	}

	/* Decode signature */
	res = base64decode(dsig, sig, sizeof(dsig));
	if (res != sizeof(dsig)) {
		ast_log(LOG_WARNING, "Signature improper length (expect %d, got %d)\n", (int)sizeof(dsig), (int)res);
		return -1;
	}

	/* Calculate digest of message */
	SHA1((unsigned char *)msg, strlen(msg), digest);

	/* Verify signature */
	res = RSA_verify(NID_sha1, digest, sizeof(digest), dsig, sizeof(dsig), key->rsa);
	
	if (!res) {
		ast_log(LOG_DEBUG, "Key failed verification\n");
		return -1;
	}
	/* Pass */
	return 0;
}

static void crypto_load(int ifd, int ofd)
{
	struct ast_key *key, *nkey, *last;
	DIR *dir;
	struct dirent *ent;
	int note = 0;
	/* Mark all keys for deletion */
	ast_mutex_lock(&keylock);
	key = keys;
	while(key) {
		key->delme = 1;
		key = key->next;
	}
	ast_mutex_unlock(&keylock);
	/* Load new keys */
	dir = opendir((char *)ast_config_AST_KEY_DIR);
	if (dir) {
		while((ent = readdir(dir))) {
			try_load_key((char *)ast_config_AST_KEY_DIR, ent->d_name, ifd, ofd, &note);
		}
		closedir(dir);
	} else
		ast_log(LOG_WARNING, "Unable to open key directory '%s'\n", (char *)ast_config_AST_KEY_DIR);
	if (note) {
		ast_log(LOG_NOTICE, "Please run the command 'init keys' to enter the passcodes for the keys\n");
	}
	ast_mutex_lock(&keylock);
	key = keys;
	last = NULL;
	while(key) {
		nkey = key->next;
		if (key->delme) {
			ast_log(LOG_DEBUG, "Deleting key %s type %d\n", key->name, key->ktype);
			/* Do the delete */
			if (last)
				last->next = nkey;
			else
				keys = nkey;
			if (key->rsa)
				RSA_free(key->rsa);
			free(key);
		} else 
			last = key;
		key = nkey;
	}
	ast_mutex_unlock(&keylock);
}

static void md52sum(char *sum, unsigned char *md5)
{
	int x;
	for (x=0;x<16;x++) 
		sum += sprintf(sum, "%02x", *(md5++));
}

static int show_keys(int fd, int argc, char *argv[])
{
	struct ast_key *key;
	char sum[16 * 2 + 1];

	ast_mutex_lock(&keylock);
	key = keys;
	ast_cli(fd, "%-18s %-8s %-16s %-33s\n", "Key Name", "Type", "Status", "Sum");
	while(key) {
		md52sum(sum, key->digest);
		ast_cli(fd, "%-18s %-8s %-16s %-33s\n", key->name, 
			(key->ktype & 0xf) == AST_KEY_PUBLIC ? "PUBLIC" : "PRIVATE",
			key->ktype & KEY_NEEDS_PASSCODE ? "[Needs Passcode]" : "[Loaded]", sum);
				
		key = key->next;
	}
	ast_mutex_unlock(&keylock);
	return RESULT_SUCCESS;
}

static int init_keys(int fd, int argc, char *argv[])
{
	struct ast_key *key;
	int ign;
	char *kn;
	char tmp[256];

	key = keys;
	while(key) {
		/* Reload keys that need pass codes now */
		if (key->ktype & KEY_NEEDS_PASSCODE) {
			kn = key->fn + strlen(ast_config_AST_KEY_DIR) + 1;
			strncpy(tmp, kn, sizeof(tmp));
			try_load_key((char *)ast_config_AST_KEY_DIR, tmp, fd, fd, &ign);
		}
		key = key->next;
	}
	return RESULT_SUCCESS;
}

static char show_key_usage[] =
"Usage: show keys\n"
"       Displays information about RSA keys known by Asterisk\n";

static char init_keys_usage[] =
"Usage: init keys\n"
"       Initializes private keys (by reading in pass code from the user)\n";

static struct ast_cli_entry cli_show_keys = 
{ { "show", "keys", NULL }, show_keys, "Displays RSA key information", show_key_usage };

static struct ast_cli_entry cli_init_keys = 
{ { "init", "keys", NULL }, init_keys, "Initialize RSA key passcodes", init_keys_usage };

static void base64_init(void)
{
	int x;
	memset(b2a, -1, sizeof(b2a));
	/* Initialize base-64 Conversion table */
	for (x=0;x<26;x++) {
		/* A-Z */
		base64[x] = 'A' + x;
		b2a['A' + x] = x;
		/* a-z */
		base64[x + 26] = 'a' + x;
		b2a['a' + x] = x + 26;
		/* 0-9 */
		if (x < 10) {
			base64[x + 52] = '0' + x;
			b2a['0' + x] = x + 52;
		}
	}
	base64[62] = '+';
	base64[63] = '/';
	b2a[(int)'+'] = 62;
	b2a[(int)'/'] = 63;
#if 0
	for (x=0;x<64;x++) {
		if (b2a[(int)base64[x]] != x) {
			fprintf(stderr, "!!! %d failed\n", x);
		} else
			fprintf(stderr, "--- %d passed\n", x);
	}
#endif
}

static int crypto_init(void)
{
	base64_init();
	SSL_library_init();
	ERR_load_crypto_strings();
	ast_cli_register(&cli_show_keys);
	ast_cli_register(&cli_init_keys);
	return 0;
}

int reload(void)
{
	crypto_load(-1, -1);
	return 0;
}

int load_module(void)
{
	crypto_init();
	if (option_initcrypto)
		crypto_load(STDIN_FILENO, STDOUT_FILENO);
	else
		crypto_load(-1, -1);
	return 0;
}

int unload_module(void)
{
	/* Can't unload this once we're loaded */
	return -1;
}

char *description(void)
{
	return "Cryptographic Digital Signatures";
}

int usecount(void)
{
	/* We should never be unloaded */
	return 1;
}

char *key()
{
	return ASTERISK_GPL_KEY;
}
