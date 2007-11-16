/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 1999 - 2006, Digium, Inc.
 *
 * Mark Spencer <markster@digium.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/*! \file
 *
 * \brief Automatic channel service routines
 *
 * \author Mark Spencer <markster@digium.com> 
 */

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include <sys/time.h>
#include <signal.h>
#include <errno.h>

#include "asterisk/pbx.h"
#include "asterisk/frame.h"
#include "asterisk/sched.h"
#include "asterisk/options.h"
#include "asterisk/channel.h"
#include "asterisk/logger.h"
#include "asterisk/file.h"
#include "asterisk/translate.h"
#include "asterisk/manager.h"
#include "asterisk/chanvars.h"
#include "asterisk/linkedlists.h"
#include "asterisk/indications.h"
#include "asterisk/lock.h"
#include "asterisk/utils.h"

#define MAX_AUTOMONS 256

struct asent {
	struct ast_channel *chan;
	AST_LIST_ENTRY(asent) list;
};

static AST_RWLIST_HEAD_STATIC(aslist, asent);

static pthread_t asthread = AST_PTHREADT_NULL;

static void *autoservice_run(void *ign)
{

	for (;;) {
		struct ast_channel *mons[MAX_AUTOMONS], *chan;
		struct asent *as;
		int x = 0, ms = 500;

		AST_RWLIST_RDLOCK(&aslist);
		AST_RWLIST_TRAVERSE(&aslist, as, list) {
			if (!ast_check_hangup(as->chan)) {
				if (x < MAX_AUTOMONS)
					mons[x++] = as->chan;
				else
					ast_log(LOG_WARNING, "Exceeded maximum number of automatic monitoring events.  Fix autoservice.c\n");
			}
		}
		AST_RWLIST_UNLOCK(&aslist);

		if ((chan = ast_waitfor_n(mons, x, &ms))) {
			/* Read and ignore anything that occurs */
			struct ast_frame *f = ast_read(chan);
			if (f)
				ast_frfree(f);
		}
	}

	asthread = AST_PTHREADT_NULL;

	return NULL;
}

int ast_autoservice_start(struct ast_channel *chan)
{
	int res = -1;
	struct asent *as;

	AST_RWLIST_WRLOCK(&aslist);

	/* Check if the channel already has autoservice */
	AST_RWLIST_TRAVERSE(&aslist, as, list) {
		if (as->chan == chan)
			break;
	}

	/* If not, start autoservice on channel */
	if (!as && (as = ast_calloc(1, sizeof(*as)))) {
		as->chan = chan;
		AST_RWLIST_INSERT_HEAD(&aslist, as, list);
		res = 0;
		if (asthread == AST_PTHREADT_NULL) { /* need start the thread */
			if (ast_pthread_create_background(&asthread, NULL, autoservice_run, NULL)) {
				ast_log(LOG_WARNING, "Unable to create autoservice thread :(\n");
				/* There will only be a single member in the list at this point,
				   the one we just added. */
				AST_RWLIST_REMOVE(&aslist, as, list);
				ast_free(as);
				res = -1;
			} else
				pthread_kill(asthread, SIGURG);
		}
	}
	AST_RWLIST_UNLOCK(&aslist);
	return res;
}

int ast_autoservice_stop(struct ast_channel *chan)
{
	int res = -1;
	struct asent *as;

	AST_RWLIST_WRLOCK(&aslist);
	AST_RWLIST_TRAVERSE_SAFE_BEGIN(&aslist, as, list) {	
		if (as->chan == chan) {
			AST_RWLIST_REMOVE_CURRENT(list);
			ast_free(as);
			if (!ast_check_hangup(chan))
				res = 0;
			break;
		}
	}
	AST_RWLIST_TRAVERSE_SAFE_END;

	if (asthread != AST_PTHREADT_NULL) 
		pthread_kill(asthread, SIGURG);
	AST_RWLIST_UNLOCK(&aslist);

	/* Wait for it to un-block */
	while (ast_test_flag(chan, AST_FLAG_BLOCKING))
		usleep(1000);

	return res;
}
