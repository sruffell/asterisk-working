/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 1999 - 2005, Digium, Inc.
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

/*! 
 * \file
 *
 * \brief Old-style G.723.1 frame/timestamp format.
 * 
 * \arg Extensions: g723, g723sf
 * \ingroup formats
 */
 
#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include "asterisk/lock.h"
#include "asterisk/channel.h"
#include "asterisk/file.h"
#include "asterisk/logger.h"
#include "asterisk/sched.h"
#include "asterisk/module.h"

#define G723_MAX_SIZE 1024

static struct ast_frame *g723_read(struct ast_filestream *s, int *whennext)
{
	unsigned short size;
	int res;
	int delay;
	/* Read the delay for the next packet, and schedule again if necessary */
	/* XXX is this ignored ? */
	if (fread(&delay, 1, 4, s->f) == 4) 
		delay = ntohl(delay);
	else
		delay = -1;
	if (fread(&size, 1, 2, s->f) != 2) {
		/* Out of data, or the file is no longer valid.  In any case
		   go ahead and stop the stream */
		return NULL;
	}
	/* Looks like we have a frame to read from here */
	size = ntohs(size);
	if (size > G723_MAX_SIZE) {
		ast_log(LOG_WARNING, "Size %d is invalid\n", size);
		/* The file is apparently no longer any good, as we
		   shouldn't ever get frames even close to this 
		   size.  */
		return NULL;
	}
	/* Read the data into the buffer */
	s->fr.frametype = AST_FRAME_VOICE;
	s->fr.subclass = AST_FORMAT_G723_1;
	s->fr.mallocd = 0;
	AST_FRAME_SET_BUFFER(&s->fr, s->buf, AST_FRIENDLY_OFFSET, size);
	if ((res = fread(s->fr.data, 1, s->fr.datalen, s->f)) != size) {
		ast_log(LOG_WARNING, "Short read (%d of %d bytes) (%s)!\n", res, size, strerror(errno));
		return NULL;
	}
	*whennext = s->fr.samples = 240;
	return &s->fr;
}

static int g723_write(struct ast_filestream *s, struct ast_frame *f)
{
	u_int32_t delay;
	u_int16_t size;
	int res;
	/* XXX there used to be a check s->fr means a read stream */
	if (f->frametype != AST_FRAME_VOICE) {
		ast_log(LOG_WARNING, "Asked to write non-voice frame!\n");
		return -1;
	}
	if (f->subclass != AST_FORMAT_G723_1) {
		ast_log(LOG_WARNING, "Asked to write non-g723 frame!\n");
		return -1;
	}
	delay = 0;
	if (f->datalen <= 0) {
		ast_log(LOG_WARNING, "Short frame ignored (%d bytes long?)\n", f->datalen);
		return 0;
	}
	if ((res = fwrite(&delay, 1, 4, s->f)) != 4) {
		ast_log(LOG_WARNING, "Unable to write delay: res=%d (%s)\n", res, strerror(errno));
		return -1;
	}
	size = htons(f->datalen);
	if ((res = fwrite(&size, 1, 2, s->f)) != 2) {
		ast_log(LOG_WARNING, "Unable to write size: res=%d (%s)\n", res, strerror(errno));
		return -1;
	}
	if ((res = fwrite(f->data, 1, f->datalen, s->f)) != f->datalen) {
		ast_log(LOG_WARNING, "Unable to write frame: res=%d (%s)\n", res, strerror(errno));
		return -1;
	}	
	return 0;
}

static int g723_seek(struct ast_filestream *fs, off_t sample_offset, int whence)
{
	return -1;
}

static int g723_trunc(struct ast_filestream *fs)
{
	/* Truncate file to current length */
	if (ftruncate(fileno(fs->f), ftello(fs->f)) < 0)
		return -1;
	return 0;
}

static off_t g723_tell(struct ast_filestream *fs)
{
	return -1;
}

static const struct ast_format g723_1_f = {
	.name = "g723sf",
	.exts = "g723|g723sf",
	.format = AST_FORMAT_G723_1,
	.write = g723_write,
	.seek =	g723_seek,
	.trunc = g723_trunc,
	.tell =	g723_tell,
	.read =	g723_read,
	.buf_size = G723_MAX_SIZE + AST_FRIENDLY_OFFSET,
};

static int load_module(void)
{
	return ast_format_register(&g723_1_f);
}

static int unload_module(void)
{
	return ast_format_unregister(g723_1_f.name);
}	

AST_MODULE_INFO_STANDARD("G.723.1 Simple Timestamp File Format", ASTERISK_GPL_KEY);
