/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Voicemail System (did you ever think it could be so easy?)
 * 
 * Copyright (C) 1999, Mark Spencer
 *
 * Mark Spencer <markster@linux-support.net>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include <asterisk/file.h>
#include <asterisk/logger.h>
#include <asterisk/channel.h>
#include <asterisk/pbx.h>
#include <asterisk/options.h>
#include <asterisk/config.h>
#include <asterisk/say.h>
#include <asterisk/module.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>

#include <pthread.h>
#include "../asterisk.h"

#define COMMAND_TIMEOUT 5000

#define VOICEMAIL_CONFIG "voicemail.conf"
#define ASTERISK_USERNAME "asterisk"

#define SENDMAIL "/usr/sbin/sendmail -t"

#define INTRO "vm-intro"

#define MAXMSG 100

#define MAX_OTHER_FORMATS 10

#define VM_SPOOL_DIR AST_SPOOL_DIR "/vm"


static char *tdesc = "Comedian Mail (Voicemail System)";

static char *synopsis_vm =
"Leave a voicemail message";

static char *descrip_vm =
"  VoiceMail([s]extension): Leaves voicemail for a given  extension (must be\n"
"configured in voicemail.conf). If the extension is preceeded by an 's' then\n"
"instructions for leaving the message will be skipped. Returns  -1 on  error\n"
"or mailbox not found, or if the user hangs up. Otherwise, it returns 0. \n";

static char *synopsis_vmain =
"Enter voicemail system";

static char *descrip_vmain =
"  VoiceMailMain(): Enters the main voicemail system for the checking of voicemail.  Returns\n"
"  -1 if the user hangs up or 0 otherwise.\n";

/* Leave a message */
static char *app = "VoiceMail";

/* Check mail, control, etc */
static char *app2 = "VoiceMailMain";

STANDARD_LOCAL_USER;

LOCAL_USER_DECL;

static int make_dir(char *dest, int len, char *ext, char *mailbox)
{
	return snprintf(dest, len, "%s/%s/%s", VM_SPOOL_DIR, ext, mailbox);
}

static int make_file(char *dest, int len, char *dir, int num)
{
	return snprintf(dest, len, "%s/msg%04d", dir, num);
}

#if 0

static int announce_message(struct ast_channel *chan, char *dir, int msgcnt)
{
	char *fn;
	int res;
	res = ast_streamfile(chan, "vm-message", chan->language);
	if (!res) {
		res = ast_waitstream(chan, AST_DIGIT_ANY);
		if (!res) {
			res = ast_say_number(chan, msgcnt+1, chan->language);
			if (!res) {
				fn = get_fn(dir, msgcnt);
				if (fn) {
					res = ast_streamfile(chan, fn, chan->language);
					free(fn);
				}
			}
		}
	}
	if (res < 0)
		ast_log(LOG_WARNING, "Unable to announce message\n");
	return res;
}
#endif

static int sendmail(char *srcemail, char *email, char *name, int msgnum, char *mailbox, char *callerid)
{
	FILE *p;
	char date[256];
	char host[256];
	char who[256];
	time_t t;
	struct tm *tm;
	p = popen(SENDMAIL, "w");
	if (p) {
		if (strchr(srcemail, '@'))
			strncpy(who, srcemail, sizeof(who));
		else {
			gethostname(host, sizeof(host));
			snprintf(who, sizeof(who), "%s@%s", srcemail, host);
		}
		time(&t);
		tm = localtime(&t);
		strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S %z", tm);
		fprintf(p, "Date: %s\n", date);
		fprintf(p, "Message-ID: <Asterisk-%d-%s-%d@%s>\n", msgnum, mailbox, getpid(), host);
		fprintf(p, "From: Asterisk PBX <%s>\n", who);
		fprintf(p, "To: %s <%s>\n", name, email);
		fprintf(p, "Subject: [PBX]: New message %d in mailbox %s\n\n", msgnum, mailbox);
		strftime(date, sizeof(date), "%A, %B %d, %Y at %r", tm);
		fprintf(p, "Dear %s:\n\n\tJust wanted to let you know you were just left a message (number %d)\n"
		           "in mailbox %s from %s, on %s so you might\n"
				   "want to check it when you get a chance.  Thanks!\n\n\t\t\t\t--Asterisk\n", name, 
			msgnum, mailbox, (callerid ? callerid : "an unknown caller"), date);
		fprintf(p, ".\n");
		pclose(p);
	} else {
		ast_log(LOG_WARNING, "Unable to launch '%s'\n", SENDMAIL);
		return -1;
	}
	return 0;
}

static int get_date(char *s, int len)
{
	struct tm *tm;
	time_t t;
	t = time(0);
	tm = localtime(&t);
	return strftime(s, len, "%a %b %e %r %Z %Y", tm);
}

static int leave_voicemail(struct ast_channel *chan, char *ext, int silent)
{
	struct ast_config *cfg;
	char *copy, *name, *passwd, *email, *fmt, *fmts;
	char comment[256];
	struct ast_filestream *writer=NULL, *others[MAX_OTHER_FORMATS];
	char *sfmt[MAX_OTHER_FORMATS];
	char txtfile[256];
	FILE *txt;
	int res = -1, fmtcnt=0, x;
	int msgnum;
	int outmsg=0;
	struct ast_frame *f;
	char date[256];
	char dir[256];
	char fn[256];
	char *astemail;
	
	cfg = ast_load(VOICEMAIL_CONFIG);
	if (!cfg) {
		ast_log(LOG_WARNING, "No such configuration file %s\n", VOICEMAIL_CONFIG);
		return -1;
	}
	if (!(astemail = ast_variable_retrieve(cfg, "general", "serveremail"))) 
		astemail = ASTERISK_USERNAME;
	if ((copy = ast_variable_retrieve(cfg, NULL, ext))) {
		/* Make sure they have an entry in the config */
		copy = strdup(copy);
		passwd = strtok(copy, ",");
		name = strtok(NULL, ",");
		email = strtok(NULL, ",");
		make_dir(dir, sizeof(dir), ext, "");
		/* It's easier just to try to make it than to check for its existence */
		if (mkdir(dir, 0700) && (errno != EEXIST))
			ast_log(LOG_WARNING, "mkdir '%s' failed: %s\n", dir, strerror(errno));
		make_dir(dir, sizeof(dir), ext, "INBOX");
		if (mkdir(dir, 0700) && (errno != EEXIST))
			ast_log(LOG_WARNING, "mkdir '%s' failed: %s\n", dir, strerror(errno));
		/* Stream an info message */
		if (silent || !ast_streamfile(chan, INTRO, chan->language)) {
			/* Wait for the message to finish */
			if (silent || !ast_waitstream(chan, "")) {
				fmt = ast_variable_retrieve(cfg, "general", "format");
				if (fmt) {
					fmts = strdup(fmt);
					fmt = strtok(fmts, "|");
					msgnum = 0;
					do {
						make_file(fn, sizeof(fn), dir, msgnum);
						snprintf(comment, sizeof(comment), "Voicemail from %s to %s (%s) on %s\n",
											(chan->callerid ? chan->callerid : "Unknown"), 
											name, ext, chan->name);
						if (ast_fileexists(fn, NULL, chan->language) > 0) {
							msgnum++;
							continue;
						}
						writer = ast_writefile(fn, fmt, comment, O_EXCL, 1 /* check for other formats */, 0700);
						if (!writer)
							break;
						msgnum++;
					} while(!writer && (msgnum < MAXMSG));
					if (writer) {
						/* Store information */
						snprintf(txtfile, sizeof(txtfile), "%s.txt", fn);
						txt = fopen(txtfile, "w+");
						if (txt) {
							get_date(date, sizeof(date));
							fprintf(txt, 
"#\n"
"# Message Information file\n"
"#\n"
"origmailbox=%s\n"
"context=%s\n"
"exten=%s\n"
"priority=%d\n"
"callerchan=%s\n"
"callerid=%s\n"
"origdate=%s\n",
	ext,
	chan->context,
	chan->exten,
	chan->priority,
	chan->name,
	chan->callerid ? chan->callerid : "Unknown",
	date);
							fclose(txt);
						} else
							ast_log(LOG_WARNING, "Error opening text file for output\n");
	
						/* We need to reset these values */
						free(fmts);
						fmt = ast_variable_retrieve(cfg, "general", "format");
						fmts = strdup(fmt);
						strtok(fmts, "|");
						while((fmt = strtok(NULL, "|"))) {
							if (fmtcnt > MAX_OTHER_FORMATS - 1) {
								ast_log(LOG_WARNING, "Please increase MAX_OTHER_FORMATS in app_voicemail.c\n");
								break;
							}
							sfmt[fmtcnt++] = strdup(fmt);
						}
						for (x=0;x<fmtcnt;x++) {
							others[x] = ast_writefile(fn, sfmt[x], comment, 0, 0, 0700);
							if (!others[x]) {
								/* Ick, the other format didn't work, but be sure not
								   to leak memory here */
								int y;
								for(y=x+1;y < fmtcnt;y++)
									free(sfmt[y]);
								break;
							}
							free(sfmt[x]);
						}
						if (x == fmtcnt) {
							/* Loop forever, writing the packets we read to the writer(s), until
							   we read a # or get a hangup */
							if (option_verbose > 2) 
								ast_verbose( VERBOSE_PREFIX_3 "Recording to %s\n", fn);
							while((f = ast_read(chan))) {
								if (f->frametype == AST_FRAME_VOICE) {
									/* Write the primary format */
									res = ast_writestream(writer, f);
									if (res) {
										ast_log(LOG_WARNING, "Error writing primary frame\n");
										break;
									}
									/* And each of the others */
									for (x=0;x<fmtcnt;x++) {
										res |= ast_writestream(others[x], f);
									}
									ast_frfree(f);
									/* Exit on any error */
									if (res) {
										ast_log(LOG_WARNING, "Error writing frame\n");
										break;
									}
								}
								if (f->frametype == AST_FRAME_DTMF) {
									if (f->subclass == '#') {
										if (option_verbose > 2) 
											ast_verbose( VERBOSE_PREFIX_3 "User ended message by pressing %c\n", f->subclass);
										outmsg=2;
										break;
									}
								}
							}
							if (!f) {
								if (option_verbose > 2) 
									ast_verbose( VERBOSE_PREFIX_3 "User hung up\n");
								res = -1;
								outmsg=1;
							}
						} else {
							ast_log(LOG_WARNING, "Error creating writestream '%s', format '%s'\n", fn, sfmt[x]); 
							free(sfmt[x]);
						}
						ast_closestream(writer);
						for (x=0;x<fmtcnt;x++) {
							if (!others[x])
								break;
							ast_closestream(others[x]);
						}
						if (outmsg) {
							if (outmsg > 1) {
								/* Let them know it worked */
								ast_streamfile(chan, "vm-msgsaved", chan->language);
								ast_waitstream(chan, "");
							}
							/* Send e-mail if applicable */
							if (email) 
								sendmail(astemail, email, name, msgnum, ext, chan->callerid);
						}
					} else {
						if (msgnum < MAXMSG)
							ast_log(LOG_WARNING, "Error writing to mailbox %s\n", ext);
						else
							ast_log(LOG_WARNING, "Too many messages in mailbox %s\n", ext);
					}
					free(fmts);
				} else 
					ast_log(LOG_WARNING, "No format to save messages in \n");
			}
		} else
			ast_log(LOG_WARNING, "Unable to playback instructions\n");
			
		free(copy);
	} else
		ast_log(LOG_WARNING, "No entry in voicemail config file for '%s'\n", ext);
	ast_destroy(cfg);
	/* Leave voicemail for someone */
	return res;
}

static char *mbox(int id)
{
	switch(id) {
	case 0:
		return "INBOX";
	case 1:
		return "Old";
	case 2:
		return "Work";
	case 3:
		return "Family";
	case 4:
		return "Friends";
	case 5:
		return "Cust1";
	case 6:
		return "Cust2";
	case 7:
		return "Cust3";
	case 8:
		return "Cust4";
	case 9:
		return "Cust5";
	default:
		return "Unknown";
	}
}

static int count_messages(char *dir)
{
	int x;
	char fn[256];
	for (x=0;x<MAXMSG;x++) {
		make_file(fn, sizeof(fn), dir, x);
		if (ast_fileexists(fn, NULL, NULL) < 1)
			break;
	}
	return x;
}

static int play_and_wait(struct ast_channel *chan, char *fn)
{
	int d;
	d = ast_streamfile(chan, fn, chan->language);
	if (d)
		return d;
	d = ast_waitstream(chan, AST_DIGIT_ANY);
	return d;
}

static int say_and_wait(struct ast_channel *chan, int num)
{
	int d;
	d = ast_say_number(chan, num, chan->language);
	return d;
}

static int copy(char *infile, char *outfile)
{
	int ifd;
	int ofd;
	int res;
	int len;
	char buf[4096];
	if ((ifd = open(infile, O_RDONLY)) < 0) {
		ast_log(LOG_WARNING, "Unable to open %s in read-only mode\n", infile);
		return -1;
	}
	if ((ofd = open(outfile, O_WRONLY | O_TRUNC | O_CREAT, 0600)) < 0) {
		ast_log(LOG_WARNING, "Unable to open %s in write-only mode\n", outfile);
		close(ifd);
		return -1;
	}
	do {
		len = read(ifd, buf, sizeof(buf));
		if (len < 0) {
			ast_log(LOG_WARNING, "Read failed on %s: %s\n", infile, strerror(errno));
			close(ifd);
			close(ofd);
			unlink(outfile);
		}
		if (len) {
			res = write(ofd, buf, len);
			if (res != len) {
				ast_log(LOG_WARNING, "Write failed on %s (%d of %d): %s\n", outfile, res, len, strerror(errno));
				close(ifd);
				close(ofd);
				unlink(outfile);
			}
		}
	} while(len);
	close(ifd);
	close(ofd);
	return 0;
}

static int save_to_folder(char *dir, int msg, char *username, int box)
{
	char sfn[256];
	char dfn[256];
	char ddir[256];
	char txt[256];
	char ntxt[256];
	char *dbox = mbox(box);
	int x;
	make_file(sfn, sizeof(sfn), dir, msg);
	make_dir(ddir, sizeof(ddir), username, dbox);
	mkdir(ddir, 0700);
	for (x=0;x<MAXMSG;x++) {
		make_file(dfn, sizeof(dfn), ddir, x);
		if (ast_fileexists(dfn, NULL, NULL) < 0)
			break;
	}
	if (x >= MAXMSG)
		return -1;
	ast_filecopy(sfn, dfn, NULL);
	if (strcmp(sfn, dfn)) {
		snprintf(txt, sizeof(txt), "%s.txt", sfn);
		snprintf(ntxt, sizeof(ntxt), "%s.txt", dfn);
		copy(txt, ntxt);
	}
	return 0;
}

static int get_folder(struct ast_channel *chan, int start)
{
	int x;
	int d;
	char fn[256];
	d = play_and_wait(chan, "vm-press");
	if (d)
		return d;
	for (x = start; x< 5; x++) {
		if ((d = ast_say_number(chan, x, chan->language)))
			return d;
		d = play_and_wait(chan, "vm-for");
		if (d)
			return d;
		snprintf(fn, sizeof(fn), "vm-%s", mbox(x));
		d = play_and_wait(chan, fn);
		if (d)
			return d;
		d = play_and_wait(chan, "vm-messages");
		if (d)
			return d;
		d = ast_waitfordigit(chan, 500);
		if (d)
			return d;
	}
	d = play_and_wait(chan, "vm-tocancel");
	if (d)
		return d;
	d = ast_waitfordigit(chan, 4000);
	return d;
}

#define WAITCMD(a) do { \
	d = (a); \
	if (d < 0) \
		goto out; \
	if (d) \
		goto cmd; \
} while(0)

#define WAITFILE2(file) do { \
	if (ast_streamfile(chan, file, chan->language)) \
		ast_log(LOG_WARNING, "Unable to play message %s\n", file); \
	d = ast_waitstream(chan, AST_DIGIT_ANY); \
	if (d < 0) { \
		goto out; \
	}\
} while(0)

#define WAITFILE(file) do { \
	if (ast_streamfile(chan, file, chan->language)) \
		ast_log(LOG_WARNING, "Unable to play message %s\n", file); \
	d = ast_waitstream(chan, AST_DIGIT_ANY); \
	if (!d) { \
		repeats = 0; \
		goto instructions; \
	} else if (d < 0) { \
		goto out; \
	} else goto cmd;\
} while(0)

#define PLAYMSG(a) do { \
	starting = 0; \
	if (!a) \
		WAITFILE2("vm-first"); \
	else if (a == lastmsg) \
		WAITFILE2("vm-last"); \
	WAITFILE2("vm-message"); \
	if (a && (a != lastmsg)) { \
		d = ast_say_number(chan, a + 1, chan->language); \
		if (d < 0) goto out; \
		if (d) goto cmd; \
	} \
	make_file(fn, sizeof(fn), curdir, a); \
	heard[a] = 1; \
	WAITFILE(fn); \
} while(0)

#define CLOSE_MAILBOX do { \
	if (lastmsg > -1) { \
		/* Get the deleted messages fixed */ \
		curmsg = -1; \
		for (x=0;x<=lastmsg;x++) { \
			if (!deleted[x] && (strcasecmp(curbox, "INBOX") || !heard[x])) { \
				/* Save this message.  It's not in INBOX or hasn't been heard */ \
				curmsg++; \
				make_file(fn, sizeof(fn), curdir, x); \
				make_file(fn2, sizeof(fn2), curdir, curmsg); \
				if (strcmp(fn, fn2)) { \
					snprintf(txt, sizeof(txt), "%s.txt", fn); \
					snprintf(ntxt, sizeof(ntxt), "%s.txt", fn2); \
					ast_filerename(fn, fn2, NULL); \
					rename(txt, ntxt); \
				} \
			} else if (!strcasecmp(curbox, "INBOX") && heard[x] && !deleted[x]) { \
				/* Move to old folder before deleting */ \
				save_to_folder(curdir, x, username, 1); \
			} \
		} \
		for (x = curmsg + 1; x<=lastmsg; x++) { \
			make_file(fn, sizeof(fn), curdir, x); \
			snprintf(txt, sizeof(txt), "%s.txt", fn); \
			ast_filedelete(fn, NULL); \
			unlink(txt); \
		} \
	} \
	memset(deleted, 0, sizeof(deleted)); \
	memset(heard, 0, sizeof(heard)); \
} while(0)

#define OPEN_MAILBOX(a) do { \
	strcpy(curbox, mbox(a)); \
	make_dir(curdir, sizeof(curdir), username, curbox); \
	lastmsg = count_messages(curdir) - 1; \
	snprintf(vmbox, sizeof(vmbox), "vm-%s", curbox); \
} while (0)

static int vm_execmain(struct ast_channel *chan, void *data)
{
	/* XXX This is, admittedly, some pretty horrendus code.  For some
	   reason it just seemed a lot easier to do with GOTO's.  I feel
	   like I'm back in my GWBASIC days. XXX */
	int res=-1;
	int valid = 0;
	char d;
	struct localuser *u;
	char username[80];
	char password[80], *copy;
	char curbox[80];
	char curdir[256];
	char vmbox[256];
	char fn[256];
	char fn2[256];
	int x;
	char ntxt[256];
	char txt[256];
	int deleted[MAXMSG] = { 0, };
	int heard[MAXMSG] = { 0, };
	int newmessages;
	int oldmessages;
	int repeats = 0;
	int curmsg = 0;
	int lastmsg = 0;
	int starting = 1;
	int box;
	struct ast_config *cfg;
	
	LOCAL_USER_ADD(u);
	cfg = ast_load(VOICEMAIL_CONFIG);
	if (!cfg) {
		ast_log(LOG_WARNING, "No voicemail configuration\n");
		goto out;
	}
	if (chan->state != AST_STATE_UP)
		ast_answer(chan);
	if (ast_streamfile(chan, "vm-login", chan->language)) {
		ast_log(LOG_WARNING, "Couldn't stream login file\n");
		goto out;
	}
	
	/* Authenticate them and get their mailbox/password */
	
	do {
		/* Prompt for, and read in the username */
		if (ast_readstring(chan, username, sizeof(username), 2000, 10000, "#") < 0) {
			ast_log(LOG_WARNING, "Couldn't read username\n");
			goto out;
		}			
		if (!strlen(username)) {
			if (option_verbose > 2)
				ast_verbose(VERBOSE_PREFIX_3 "Username not entered\n");
			res = 0;
			goto out;
		}
		if (ast_streamfile(chan, "vm-password", chan->language)) {
			ast_log(LOG_WARNING, "Unable to stream password file\n");
			goto out;
		}
		if (ast_readstring(chan, password, sizeof(password), 2000, 10000, "#") < 0) {
			ast_log(LOG_WARNING, "Unable to read password\n");
			goto out;
		}
		copy = ast_variable_retrieve(cfg, NULL, username);
		if (copy) {
			copy = strdup(copy);
			strtok(copy, ",");
			if (!strcmp(password,copy))
				valid++;
			else if (option_verbose > 2)
				ast_verbose( VERBOSE_PREFIX_3 "Incorrect password '%s' for user '%s'\n", password, username);
			free(copy);
		} else if (option_verbose > 2)
			ast_verbose( VERBOSE_PREFIX_3 "No such user '%s' in config file\n", username);
		if (!valid) {
			if (ast_streamfile(chan, "vm-incorrect", chan->language))
				break;
			if (ast_waitstream(chan, ""))
				break;
		}
	} while (!valid);

	if (valid) {
		OPEN_MAILBOX(1);
		oldmessages = lastmsg + 1;
		/* Start in INBOX */
		OPEN_MAILBOX(0);
		newmessages = lastmsg + 1;
		
		WAITCMD(play_and_wait(chan, "vm-youhave"));
		if (newmessages) {
			WAITCMD(say_and_wait(chan, newmessages));
			WAITCMD(play_and_wait(chan, "vm-INBOX"));
			if (newmessages == 1)
				WAITCMD(play_and_wait(chan, "vm-message"));
			else
				WAITCMD(play_and_wait(chan, "vm-messages"));
				
			if (oldmessages)
				WAITCMD(play_and_wait(chan, "vm-and"));
		}
		if (oldmessages) {
			WAITCMD(say_and_wait(chan, oldmessages));
			WAITCMD(play_and_wait(chan, "vm-Old"));
			if (oldmessages == 1)
				WAITCMD(play_and_wait(chan, "vm-message"));
			else
				WAITCMD(play_and_wait(chan, "vm-messages"));
		}
		if (!oldmessages && !newmessages) {
			WAITCMD(play_and_wait(chan, "vm-no"));
			WAITCMD(play_and_wait(chan, "vm-messages"));
		}
		if (!newmessages && oldmessages) {
			/* If we only have old messages start here */
			OPEN_MAILBOX(1);
		}
		repeats = 0;
		starting = 1;
instructions:
		if (starting) {
			if (lastmsg > -1) {
				WAITCMD(play_and_wait(chan, "vm-onefor"));
				WAITCMD(play_and_wait(chan, vmbox));
				WAITCMD(play_and_wait(chan, "vm-messages"));
			}
			WAITCMD(play_and_wait(chan, "vm-opts"));
		} else {
			if (curmsg)
				WAITCMD(play_and_wait(chan, "vm-prev"));
			WAITCMD(play_and_wait(chan, "vm-repeat"));
			if (curmsg != lastmsg)
				WAITCMD(play_and_wait(chan, "vm-next"));
			if (!deleted[curmsg])
				WAITCMD(play_and_wait(chan, "vm-delete"));
			else
				WAITCMD(play_and_wait(chan, "vm-undelete"));
			WAITCMD(play_and_wait(chan, "vm-toforward"));
			WAITCMD(play_and_wait(chan, "vm-savemessage"));
		}
		WAITCMD(play_and_wait(chan, "vm-helpexit"));
		d = ast_waitfordigit(chan, 6000);
		if (d < 0)
			goto out;
		if (!d) {
			repeats++;
			if (repeats > 2) {
				play_and_wait(chan, "vm-goodbye");
				goto out;
			}
			goto instructions;
		}
cmd:
		switch(d) {
		case '2':
			box = play_and_wait(chan, "vm-changeto");
			if (box < 0)
				goto out;
			while((box < '0') || (box > '9')) {
				box = get_folder(chan, 0);
				if (box < 0)
					goto out;
				if (box == '#')
					goto instructions;
			} 
			box = box - '0';
			CLOSE_MAILBOX;
			OPEN_MAILBOX(box);
			WAITCMD(play_and_wait(chan, vmbox));
			WAITCMD(play_and_wait(chan, "vm-messages"));
			starting = 1;
			goto instructions;
		case '4':
			if (curmsg) {
				curmsg--;
				PLAYMSG(curmsg);
			} else {
				WAITCMD(play_and_wait(chan, "vm-nomore"));
				goto instructions;
			}
		case '1':
				curmsg = 0;
				/* Fall through */
		case '5':
			if (lastmsg > -1) {
				PLAYMSG(curmsg);
			} else {
				WAITCMD(play_and_wait(chan, "vm-youhave"));
				WAITCMD(play_and_wait(chan, "vm-no"));
				snprintf(fn, sizeof(fn), "vm-%s", curbox);
				WAITCMD(play_and_wait(chan, fn));
				WAITCMD(play_and_wait(chan, "vm-messages"));
				goto instructions;
			}
		case '6':
			if (curmsg < lastmsg) {
				curmsg++;
				PLAYMSG(curmsg);
			} else {
				WAITCMD(play_and_wait(chan, "vm-nomore"));
				goto instructions;
			}
		case '7':
			deleted[curmsg] = !deleted[curmsg];
			if (deleted[curmsg]) 
				WAITCMD(play_and_wait(chan, "vm-deleted"));
			else
				WAITCMD(play_and_wait(chan, "vm-undeleted"));
			goto instructions;
		case '9':
			box = play_and_wait(chan, "vm-savefolder");
			if (box < 0)
				goto out;
			while((box < '1') || (box > '9')) {
				box = get_folder(chan, 1);
				if (box < 0)
					goto out;
				if (box == '#')
					goto instructions;
			} 
			box = box - '0';
			ast_log(LOG_DEBUG, "Save to folder: %s (%d)\n", mbox(box), box);
			if (save_to_folder(curdir, curmsg, username, box))
				goto out;
			deleted[curmsg]=1;
			WAITCMD(play_and_wait(chan, "vm-message"));
			WAITCMD(say_and_wait(chan, curmsg + 1) );
			WAITCMD(play_and_wait(chan, "vm-savedto"));
			snprintf(fn, sizeof(fn), "vm-%s", mbox(box));
			WAITCMD(play_and_wait(chan, fn));
			WAITCMD(play_and_wait(chan, "vm-messages"));
			goto instructions;
		case '*':
			if (!starting) {
				WAITCMD(play_and_wait(chan, "vm-onefor"));
				WAITCMD(play_and_wait(chan, vmbox));
				WAITCMD(play_and_wait(chan, "vm-messages"));
				WAITCMD(play_and_wait(chan, "vm-opts"));
			}
			goto instructions;
		case '#':
			play_and_wait(chan, "vm-goodbye");
			goto out;
		default:
			goto instructions;
		}
	}
out:
	CLOSE_MAILBOX;
	ast_stopstream(chan);
	if (cfg)
		ast_destroy(cfg);
	LOCAL_USER_REMOVE(u);
	return res;
}

static int vm_exec(struct ast_channel *chan, void *data)
{
	int res=0, silent=0;
	struct localuser *u;
	char *ext = (char *)data;
	
	if (!data) {
		ast_log(LOG_WARNING, "vm requires an argument (extension)\n");
		return -1;
	}
	LOCAL_USER_ADD(u);
	if (*ext == 's') {
		silent++;
		ext++;
	}
	if (chan->state != AST_STATE_UP)
		ast_answer(chan);
	res = leave_voicemail(chan, ext, silent);
	LOCAL_USER_REMOVE(u);
	return res;
}

int unload_module(void)
{
	int res;
	STANDARD_HANGUP_LOCALUSERS;
	res = ast_unregister_application(app);
	res |= ast_unregister_application(app2);
	return res;
}

int load_module(void)
{
	int res;
	res = ast_register_application(app, vm_exec, synopsis_vm, descrip_vm);
	if (!res)
		res = ast_register_application(app2, vm_execmain, synopsis_vmain, descrip_vmain);
	return res;
}

char *description(void)
{
	return tdesc;
}

int usecount(void)
{
	int res;
	STANDARD_USECOUNT(res);
	return res;
}

char *key()
{
	return ASTERISK_GPL_KEY;
}
