/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Local Proxy Channel
 * 
 * Copyright (C) 1999, Mark Spencer
 *
 * Mark Spencer <markster@linux-support.net>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <asterisk/lock.h>
#include <asterisk/channel.h>
#include <asterisk/channel_pvt.h>
#include <asterisk/config.h>
#include <asterisk/logger.h>
#include <asterisk/module.h>
#include <asterisk/pbx.h>
#include <asterisk/options.h>
#include <asterisk/lock.h>
#include <asterisk/sched.h>
#include <asterisk/io.h>
#include <asterisk/rtp.h>
#include <asterisk/acl.h>
#include <asterisk/callerid.h>
#include <asterisk/file.h>
#include <asterisk/cli.h>
#include <asterisk/app.h>
#include <asterisk/musiconhold.h>
#include <asterisk/manager.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/signal.h>

static char *desc = "Local Proxy Channel";
static char *type = "Local";
static char *tdesc = "Local Proxy Channel Driver";

static int capability = -1;

static int usecnt =0;
static pthread_mutex_t usecnt_lock = AST_MUTEX_INITIALIZER;

#define IS_OUTBOUND(a,b) (a == b->chan ? 1 : 0)

/* Protect the interface list (of sip_pvt's) */
static pthread_mutex_t locallock = AST_MUTEX_INITIALIZER;

static struct local_pvt {
	pthread_mutex_t lock;				/* Channel private lock */
	char context[AST_MAX_EXTENSION];	/* Context to call */
	char exten[AST_MAX_EXTENSION];		/* Extension to call */
	int reqformat;						/* Requested format */
	struct ast_channel *owner;			/* Master Channel */
	struct ast_channel *chan;			/* Outbound channel */
	struct local_pvt *next;				/* Next entity */
} *locals = NULL;

static int local_queue_frame(struct local_pvt *p, int isoutbound, struct ast_frame *f)
{
	struct ast_channel *other;
	if (isoutbound)
		other = p->owner;
	else
		other = p->chan;
	if (!other)
		return 0;
retrylock:		
	if (pthread_mutex_trylock(&other->lock)) {
		/* Failed to lock.  Release main lock and try again */
		ast_pthread_mutex_unlock(&p->lock);
		/* Wait just a bit */
		usleep(1);
		ast_pthread_mutex_lock(&p->lock);
		goto retrylock;
	}
	ast_queue_frame(other, f, 0);
	ast_pthread_mutex_unlock(&other->lock);
	return 0;
}

static int local_answer(struct ast_channel *ast)
{
	struct local_pvt *p = ast->pvt->pvt;
	int isoutbound = IS_OUTBOUND(ast, p);
	int res = -1;
	ast_pthread_mutex_lock(&p->lock);
	if (isoutbound) {
		/* Pass along answer since somebody answered us */
		struct ast_frame answer = { AST_FRAME_CONTROL, AST_CONTROL_ANSWER };
		local_queue_frame(p, isoutbound, &answer);
	} else
		ast_log(LOG_WARNING, "Huh?  Local is being asked to answer?\n");
	ast_pthread_mutex_unlock(&p->lock);
	return res;
}

static struct ast_frame  *local_read(struct ast_channel *ast)
{
	static struct ast_frame null = { AST_FRAME_NULL, };
	return &null;
}

static int local_write(struct ast_channel *ast, struct ast_frame *f)
{
	struct local_pvt *p = ast->pvt->pvt;
	int res = -1;
	int isoutbound = IS_OUTBOUND(ast, p);

	/* Just queue for delivery to the other side */
	ast_pthread_mutex_lock(&p->lock);
	res = local_queue_frame(p, isoutbound, f);
	ast_pthread_mutex_unlock(&p->lock);
	return res;
}

static int local_fixup(struct ast_channel *oldchan, struct ast_channel *newchan)
{
	struct local_pvt *p = newchan->pvt->pvt;
	ast_pthread_mutex_lock(&p->lock);
	if ((p->owner != oldchan) && (p->chan != oldchan)) {
		ast_log(LOG_WARNING, "old channel wasn't %p but was %p/%p\n", oldchan, p->owner, p->chan);
		ast_pthread_mutex_unlock(&p->lock);
		return -1;
	}
	if (p->owner == oldchan)
		p->owner = newchan;
	else
		p->chan = newchan;	
	ast_pthread_mutex_unlock(&p->lock);
	return 0;
}

static int local_indicate(struct ast_channel *ast, int condition)
{
	struct local_pvt *p = ast->pvt->pvt;
	int res = -1;
	struct ast_frame f = { AST_FRAME_CONTROL, };
	int isoutbound = IS_OUTBOUND(ast, p);
	/* Queue up a frame representing the indication as a control frame */
	ast_pthread_mutex_lock(&p->lock);
	f.subclass = condition;
	res = local_queue_frame(p, isoutbound, &f);
	ast_pthread_mutex_unlock(&p->lock);
	return res;
}

static int local_digit(struct ast_channel *ast, char digit)
{
	struct local_pvt *p = ast->pvt->pvt;
	int res = -1;
	struct ast_frame f = { AST_FRAME_DTMF, };
	int isoutbound = IS_OUTBOUND(ast, p);
	ast_pthread_mutex_lock(&p->lock);
	f.subclass = digit;
	res = local_queue_frame(p, isoutbound, &f);
	ast_pthread_mutex_unlock(&p->lock);
	return res;
}

static int local_call(struct ast_channel *ast, char *dest, int timeout)
{
	struct local_pvt *p = ast->pvt->pvt;
	/* Start switch on sub channel */
	return ast_pbx_start(p->chan);
}

static void local_destroy(struct local_pvt *p)
{
	struct local_pvt *cur, *prev = NULL;
	ast_pthread_mutex_lock(&locallock);
	cur = locals;
	while(cur) {
		if (cur == p) {
			if (prev)
				prev->next = cur->next;
			else
				locals = cur->next;
			free(cur);
			break;
		}
		prev = cur;
		cur = cur->next;
	}
	ast_pthread_mutex_unlock(&locallock);
	if (!cur)
		ast_log(LOG_WARNING, "Unable ot find local '%s@%s' in local list\n", p->exten, p->context);
}

static int local_hangup(struct ast_channel *ast)
{
	struct local_pvt *p = ast->pvt->pvt;
	int isoutbound = IS_OUTBOUND(ast, p);
	struct ast_frame f = { AST_FRAME_CONTROL, AST_CONTROL_HANGUP };
	ast_pthread_mutex_lock(&p->lock);
	if (isoutbound)
		p->chan = NULL;
	else
		p->owner = NULL;
	ast->pvt->pvt = NULL;
	if (!p->owner && !p->chan) {
		/* Okay, done with the private part now, too. */
		ast_pthread_mutex_unlock(&p->lock);
		free(p);
		return 0;
	}
	local_queue_frame(p, isoutbound, &f);
	ast_pthread_mutex_unlock(&p->lock);
	return 0;
}

static struct local_pvt *local_alloc(char *data, int format)
{
	struct local_pvt *tmp;
	char *c;
	tmp = malloc(sizeof(struct local_pvt));
	if (tmp) {
		memset(tmp, 0, sizeof(struct local_pvt));
		ast_pthread_mutex_init(&tmp->lock);
		strncpy(tmp->exten, data, sizeof(tmp->exten) - 1);
		c = strchr(tmp->exten, '@');
		if (c) {
			*c = '\0';
			c++;
			strncpy(tmp->context, data, sizeof(tmp->context) - 1);
		}
		tmp->reqformat = format;
		/* Add to list */
		ast_pthread_mutex_lock(&locallock);
		tmp->next = locals;
		locals = tmp;
		ast_pthread_mutex_unlock(&locallock);
		
	}
	return tmp;
}

static struct ast_channel *local_new(struct local_pvt *p, int state)
{
	struct ast_channel *tmp, *tmp2;
	tmp = ast_channel_alloc(1);
	tmp2 = ast_channel_alloc(1);
	if (!tmp || !tmp2) {
		if (tmp)
			ast_channel_free(tmp);
		if (tmp2)
			ast_channel_free(tmp2);
		tmp = NULL;
	}
	if (tmp) {
		tmp->nativeformats = p->reqformat;
		tmp->nativeformats = p->reqformat;
		snprintf(tmp->name, sizeof(tmp->name), "Local/%s@%s-1", p->exten, p->context);
		snprintf(tmp2->name, sizeof(tmp2->name), "Local/%s@%s-2", p->exten, p->context);
		tmp->type = type;
		tmp2->type = type;
		ast_setstate(tmp, state);
		ast_setstate(tmp2, AST_STATE_RING);
		tmp->writeformat = p->reqformat;;
		tmp2->writeformat = p->reqformat;
		tmp->pvt->rawwriteformat = p->reqformat;
		tmp2->pvt->rawwriteformat = p->reqformat;
		tmp->readformat = p->reqformat;
		tmp2->readformat = p->reqformat;
		tmp->pvt->rawreadformat = p->reqformat;
		tmp2->pvt->rawreadformat = p->reqformat;
		tmp->pvt->pvt = p;
		tmp2->pvt->pvt = p;
		tmp->pvt->send_digit = local_digit;
		tmp2->pvt->send_digit = local_digit;
		tmp->pvt->call = local_call;
		tmp2->pvt->call = local_call;
		tmp->pvt->hangup = local_hangup;
		tmp2->pvt->hangup = local_hangup;
		tmp->pvt->answer = local_answer;
		tmp2->pvt->answer = local_answer;
		tmp->pvt->read = local_read;
		tmp2->pvt->read = local_read;
		tmp->pvt->write = local_write;
		tmp2->pvt->write = local_write;
		tmp->pvt->exception = local_read;
		tmp2->pvt->exception = local_read;
		tmp->pvt->indicate = local_indicate;
		tmp2->pvt->indicate = local_indicate;
		tmp->pvt->fixup = local_fixup;
		tmp2->pvt->fixup = local_fixup;
		p->owner = tmp;
		p->chan = tmp2;
		ast_pthread_mutex_lock(&usecnt_lock);
		usecnt++;
		ast_pthread_mutex_unlock(&usecnt_lock);
		ast_update_use_count();
		strncpy(tmp->context, p->context, sizeof(tmp->context)-1);
		strncpy(tmp2->context, p->context, sizeof(tmp2->context)-1);
		strncpy(tmp2->exten, p->exten, sizeof(tmp->exten)-1);
		tmp->priority = 1;
		tmp2->priority = 1;
	} else
		ast_log(LOG_WARNING, "Unable to allocate channel structure\n");
	return tmp;
}


static struct ast_channel *local_request(char *type, int format, void *data)
{
	struct local_pvt *p;
	struct ast_channel *chan = NULL;
	p = local_alloc(data, format);
	chan = local_new(p, AST_STATE_DOWN);
	return chan;
}

static int locals_show(int fd, int argc, char **argv)
{
	struct local_pvt *p;

	if (argc != 2)
		return RESULT_SHOWUSAGE;
	ast_pthread_mutex_lock(&locallock);
	p = locals;
	while(p) {
		ast_pthread_mutex_lock(&p->lock);
		ast_cli(fd, "%s -- %s@%s\n", p->owner ? p->owner->name : "<unowned>", p->exten, p->context);
		ast_pthread_mutex_unlock(&p->lock);
		p = p->next;
	}
	if (!locals)
		ast_cli(fd, "No local channels in use\n");
	ast_pthread_mutex_unlock(&locallock);
	return RESULT_SUCCESS;
}

static char show_locals_usage[] = 
"Usage: show locals\n"
"       Provides summary information on locals.\n";

static struct ast_cli_entry cli_show_locals = {
	{ "show", "locals", NULL }, locals_show, 
	"Show status of local channels", show_locals_usage, NULL };

int load_module()
{
	/* Make sure we can register our sip channel type */
	if (ast_channel_register(type, tdesc, capability, local_request)) {
		ast_log(LOG_ERROR, "Unable to register channel class %s\n", type);
		return -1;
	}
	ast_cli_register(&cli_show_locals);
	return 0;
}

int reload()
{
	return 0;
}

int unload_module()
{
	struct local_pvt *p;
	/* First, take us out of the channel loop */
	ast_cli_unregister(&cli_show_locals);
	ast_channel_unregister(type);
	if (!ast_pthread_mutex_lock(&locallock)) {
		/* Hangup all interfaces if they have an owner */
		p = locals;
		while(p) {
			if (p->owner)
				ast_softhangup(p->owner, AST_SOFTHANGUP_APPUNLOAD);
			p = p->next;
		}
		locals = NULL;
		ast_pthread_mutex_unlock(&locallock);
	} else {
		ast_log(LOG_WARNING, "Unable to lock the monitor\n");
		return -1;
	}		
	return 0;
}

int usecount()
{
	int res;
	ast_pthread_mutex_lock(&usecnt_lock);
	res = usecnt;
	ast_pthread_mutex_unlock(&usecnt_lock);
	return res;
}

char *key()
{
	return ASTERISK_GPL_KEY;
}

char *description()
{
	return desc;
}

