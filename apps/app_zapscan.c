/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Zap Scanner
 *
 * Copyright (C) 2003, Digium
 *
 * Modified from app_zapbarge by David Troy <dave@toad.net>
 *
 * Mark Spencer <markster@digium.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 *
 * Special thanks to comphealth.com for sponsoring this
 * GPL application.
 */

#include <asterisk/lock.h>
#include <asterisk/file.h>
#include <asterisk/logger.h>
#include <asterisk/channel.h>
#include <asterisk/pbx.h>
#include <asterisk/module.h>
#include <asterisk/config.h>
#include <asterisk/app.h>
#include <asterisk/options.h>
#include <asterisk/cli.h>
#include <asterisk/say.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <pthread.h>
#include <linux/zaptel.h>
static char *tdesc = "Scan Zap channels application";

static char *app = "ZapScan";

static char *synopsis = "Scan Zap channels to monitor calls";

static char *descrip =
"  ZapScan allows a call center manager to monitor Zap channels in\n"
"a convenient way.  Use '#' to select the next channel and use '*' to exit\n";


STANDARD_LOCAL_USER;

LOCAL_USER_DECL;


#define CONF_SIZE 160

static struct ast_channel *get_zap_channel(int num) {
	struct ast_channel *c=NULL;
	char name[80];

	snprintf(name,sizeof(name),"Zap/%d-1",num);
	c = ast_channel_walk(NULL);
	while(c) {
		if (!strcasecmp(c->name, name)) {
			break;
		}
		c = ast_channel_walk(c);
	}
	if (c)
		return c;

	return NULL;
}

static int careful_write(int fd, unsigned char *data, int len)
{
        int res;
        while(len) {
                res = write(fd, data, len);
                if (res < 1) {
                        if (errno != EAGAIN) {
                                ast_log(LOG_WARNING, "Failed to write audio data to conference: %s\n", strerror(errno));
                                return -1;
                        } else
                                return 0;
                }
                len -= res;
                data += res;
        }
        return 0;
}

static int conf_run(struct ast_channel *chan, int confno, int confflags)
{
        int fd;
        struct zt_confinfo ztc;
        struct ast_frame *f;
        struct ast_channel *c;
        struct ast_frame fr;
        int outfd;
        int ms;
        int nfds;
        int res;
        int flags;
        int retryzap;
        int origfd;
        int ret = -1;
		char input[4];
		int ic=0;

        ZT_BUFFERINFO bi;
        char __buf[CONF_SIZE + AST_FRIENDLY_OFFSET];
        char *buf = __buf + AST_FRIENDLY_OFFSET;

        /* Set it into U-law mode (write) */
        if (ast_set_write_format(chan, AST_FORMAT_ULAW) < 0) {
                ast_log(LOG_WARNING, "Unable to set '%s' to write ulaw mode\n", chan->name);
                goto outrun;
        }

        /* Set it into U-law mode (read) */
        if (ast_set_read_format(chan, AST_FORMAT_ULAW) < 0) {
                ast_log(LOG_WARNING, "Unable to set '%s' to read ulaw mode\n", chan->name);
                goto outrun;
        }
        ast_indicate(chan, -1);
        retryzap = strcasecmp(chan->type, "Zap");
zapretry:
        origfd = chan->fds[0];
        if (retryzap) {
                fd = open("/dev/zap/pseudo", O_RDWR);
                if (fd < 0) {
                        ast_log(LOG_WARNING, "Unable to open pseudo channel: %s\n", strerror(errno));
                        goto outrun;
                }
                /* Make non-blocking */
                flags = fcntl(fd, F_GETFL);
                if (flags < 0) {
                        ast_log(LOG_WARNING, "Unable to get flags: %s\n", strerror(errno));
                        close(fd);
                        goto outrun;
                }
                if (fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
                        ast_log(LOG_WARNING, "Unable to set flags: %s\n", strerror(errno));
                        close(fd);
                        goto outrun;
                }
                /* Setup buffering information */
                memset(&bi, 0, sizeof(bi));
                bi.bufsize = CONF_SIZE;
                bi.txbufpolicy = ZT_POLICY_IMMEDIATE;
                bi.rxbufpolicy = ZT_POLICY_IMMEDIATE;
                bi.numbufs = 4;
                if (ioctl(fd, ZT_SET_BUFINFO, &bi)) {
                        ast_log(LOG_WARNING, "Unable to set buffering information: %s\n", strerror(errno));
                        close(fd);
                        goto outrun;
                }
                nfds = 1;
        } else {
                /* XXX Make sure we're not running on a pseudo channel XXX */
                fd = chan->fds[0];
                nfds = 0;
        }
        memset(&ztc, 0, sizeof(ztc));
        /* Check to see if we're in a conference... */
        ztc.chan = 0;
        if (ioctl(fd, ZT_GETCONF, &ztc)) {
                ast_log(LOG_WARNING, "Error getting conference\n");
                close(fd);
                goto outrun;
        }
        if (ztc.confmode) {
                /* Whoa, already in a conference...  Retry... */
                if (!retryzap) {
                        ast_log(LOG_DEBUG, "Zap channel is in a conference already, retrying with pseudo\n");
                        retryzap = 1;
                        goto zapretry;
                }
        }
        memset(&ztc, 0, sizeof(ztc));
        /* Add us to the conference */
        ztc.chan = 0;
        ztc.confno = confno;
        ztc.confmode = ZT_CONF_MONITORBOTH;

        if (ioctl(fd, ZT_SETCONF, &ztc)) {
                ast_log(LOG_WARNING, "Error setting conference\n");
                close(fd);
                goto outrun;
        }
        ast_log(LOG_DEBUG, "Placed channel %s in ZAP channel %d monitor\n", chan->name, confno);

        for(;;) {
                outfd = -1;
                ms = -1;
                c = ast_waitfor_nandfds(&chan, 1, &fd, nfds, NULL, &outfd, &ms);
                if (c) {
                        if (c->fds[0] != origfd) {
                                if (retryzap) {
                                        /* Kill old pseudo */
                                        close(fd);
                                }
                                ast_log(LOG_DEBUG, "Ooh, something swapped out under us, starting over\n");
                                retryzap = 0;
                                goto zapretry;
                        }
                        f = ast_read(c);
                        if (!f)
                                break;
						if(f->frametype == AST_FRAME_DTMF) {
							if(f->subclass == '#') {
								ret = 0;
                                break;
							}
							else if (f->subclass == '*') {
								ret = -1;
								break;
								
							}
							else {
								input[ic++] = f->subclass;
							}
							if(ic == 3) {
								input[ic++] = '\0';
								ic=0;
								ret = atoi(input);
								ast_verbose(VERBOSE_PREFIX_3 "Zapscan: change channel to %d\n",ret);
								break;
							}
						}

						if (fd != chan->fds[0]) {
							if (f->frametype == AST_FRAME_VOICE) {
                                        if (f->subclass == AST_FORMAT_ULAW) {
                                                /* Carefully write */
                                                careful_write(fd, f->data, f->datalen);
                                        } else
                                                ast_log(LOG_WARNING, "Huh?  Got a non-ulaw (%d) frame in the conference\n", f->subclass);
                                }
                        }
                        ast_frfree(f);
                } else if (outfd > -1) {
                        res = read(outfd, buf, CONF_SIZE);
                        if (res > 0) {
                                memset(&fr, 0, sizeof(fr));
                                fr.frametype = AST_FRAME_VOICE;
                                fr.subclass = AST_FORMAT_ULAW;
                                fr.datalen = res;
                                fr.samples = res;
                                fr.data = buf;
                                fr.offset = AST_FRIENDLY_OFFSET;
                                if (ast_write(chan, &fr) < 0) {
                                        ast_log(LOG_WARNING, "Unable to write frame to channel: %s\n", strerror(errno));
                                        /* break; */
                                }
                        } else
                                ast_log(LOG_WARNING, "Failed to read frame: %s\n", strerror(errno));
                }
        }
        if (fd != chan->fds[0])
                close(fd);
        else {
                /* Take out of conference */
                /* Add us to the conference */
                ztc.chan = 0;
                ztc.confno = 0;
                ztc.confmode = 0;
                if (ioctl(fd, ZT_SETCONF, &ztc)) {
                        ast_log(LOG_WARNING, "Error setting conference\n");
                }
        }

outrun:

        return ret;
}

static int conf_exec(struct ast_channel *chan, void *data)
{
        int res=-1;
        struct localuser *u;
        int confflags = 0;
        int confno = 0;
        char confstr[80], *tmp;
        struct ast_channel *tempchan = NULL, *lastchan = NULL,*ichan = NULL;
		struct ast_frame *f;
		int input=0;
		
        LOCAL_USER_ADD(u);

        if (chan->_state != AST_STATE_UP)
                ast_answer(chan);

        for (;;) {
		if (ast_waitfor(chan, 100) < 0)
			break;
		f = ast_read(chan);
		if (!f)
			break;
		if ((f->frametype == AST_FRAME_DTMF) && (f->subclass == '*')) {
			ast_frfree(f);
			break;
		}
		ast_frfree(f);
		ichan = NULL;
		if(input) {
			ichan = get_zap_channel(input);
			input = 0;
		}

		tempchan = ichan ? ichan : ast_channel_walk(tempchan);
		
		
		if ( !tempchan && !lastchan )
			break;
		if ( tempchan && tempchan->type && (!strcmp(tempchan->type, "Zap")) && (tempchan != chan) ) {
			ast_verbose(VERBOSE_PREFIX_3 "Zap channel %s is in-use, monitoring...\n", tempchan->name);
			strcpy(confstr, tempchan->name);
			if ((tmp = strchr(confstr,'-'))) {
				*tmp = '\0';
			}
			confno = atoi(strchr(confstr,'/') + 1);
			ast_stopstream(chan);
			ast_say_number(chan, confno, AST_DIGIT_ANY, chan->language, (char *) NULL);
			res = conf_run(chan, confno, confflags);
			if (res<0) break;
			input = res;
		}
		lastchan = tempchan;
        }

        LOCAL_USER_REMOVE(u);
        return res;
}

int unload_module(void)
{
        STANDARD_HANGUP_LOCALUSERS;
        return ast_unregister_application(app);
}

int load_module(void)
{
        return ast_register_application(app, conf_exec, synopsis, descrip);
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

