/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Execute arbitrary system commands
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
#include <asterisk/module.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>


static char *tdesc = "Generic System() application";

static char *app = "System";

STANDARD_LOCAL_USER;

LOCAL_USER_DECL;

static int skel_exec(struct ast_channel *chan, void *data)
{
	int res=0;
	struct localuser *u;
	if (!data) {
		ast_log(LOG_WARNING, "System requires an argument(command)\n");
		return -1;
	}
	LOCAL_USER_ADD(u);
	/* Do our thing here */
	res = system((char *)data);
	if (res < 0) {
		ast_log(LOG_WARNING, "Unable to execute '%s'\n", data);
		res = -1;
	} else if (res == 127) {
		ast_log(LOG_WARNING, "Unable to execute '%s'\n", data);
		res = -1;
	} else {
		if (res && ast_exists_extension(chan, chan->context, chan->exten, chan->priority + 101)) 
			chan->priority+=100;
		res = 0;
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
	return ast_register_application(app, skel_exec);
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
