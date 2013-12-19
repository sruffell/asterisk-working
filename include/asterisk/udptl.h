/*
 * Asterisk -- An open source telephony toolkit.
 *
 * UDPTL support for T.38
 * 
 * Copyright (C) 2005, Steve Underwood, partly based on RTP code which is
 * Copyright (C) 1999-2004, Digium, Inc.
 *
 * Steve Underwood <steveu@coppice.org>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 *
 * A license has been granted to Digium (via disclaimer) for the use of
 * this code.
 */

/*! \file
 * \brief UDPTL support for T.38
 * \author Steve Underwood <steveu@coppice.org>
 * \ref udptl.c
 * \todo add doxygen documentation to this file!
 */


#ifndef _ASTERISK_UDPTL_H
#define _ASTERISK_UDPTL_H

#include "asterisk/network.h"
#include "asterisk/frame.h"
#include "asterisk/io.h"
#include "asterisk/sched.h"
#include "asterisk/channel.h"
#include "asterisk/netsock2.h"


enum ast_t38_ec_modes {
    UDPTL_ERROR_CORRECTION_NONE,
    UDPTL_ERROR_CORRECTION_FEC,
    UDPTL_ERROR_CORRECTION_REDUNDANCY
};

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

struct ast_udptl;

typedef int (*ast_udptl_callback)(struct ast_udptl *udptl, struct ast_frame *f, void *data);

struct ast_udptl *ast_udptl_new_with_bindaddr(struct ast_sched_context *sched, struct io_context *io, int callbackmode, struct ast_sockaddr *in);

/*!
 * \brief Associates a character string 'tag' with a UDPTL session.
 * \param udptl The UDPTL session.
 * \param format printf-style format string used to construct the tag
 * 
 * This function formats a tag for the specified UDPTL
 * session, so that any log messages generated by the UDPTL stack
 * related to that session will include the tag and the reader of
 * the messages will be able to identify which endpoint caused them
 * to be generated.
 *
 * \retval none
 */
void __attribute__((format(printf, 2, 3))) ast_udptl_set_tag(struct ast_udptl *udptl, const char *format, ...);

void ast_udptl_set_peer(struct ast_udptl *udptl, const struct ast_sockaddr *them);

void ast_udptl_get_peer(const struct ast_udptl *udptl, struct ast_sockaddr *them);

void ast_udptl_get_us(const struct ast_udptl *udptl, struct ast_sockaddr *us);

void ast_udptl_destroy(struct ast_udptl *udptl);

void ast_udptl_reset(struct ast_udptl *udptl);

void ast_udptl_set_callback(struct ast_udptl *udptl, ast_udptl_callback callback);

void ast_udptl_set_data(struct ast_udptl *udptl, void *data);

int ast_udptl_write(struct ast_udptl *udptl, struct ast_frame *f);

struct ast_frame *ast_udptl_read(struct ast_udptl *udptl);

int ast_udptl_fd(const struct ast_udptl *udptl);

int ast_udptl_setqos(struct ast_udptl *udptl, unsigned int tos, unsigned int cos);

void ast_udptl_set_m_type(struct ast_udptl *udptl, unsigned int pt);

void ast_udptl_set_udptlmap_type(struct ast_udptl *udptl, unsigned int pt,
				 char *mimeType, char *mimeSubtype);

enum ast_t38_ec_modes ast_udptl_get_error_correction_scheme(const struct ast_udptl *udptl);

void ast_udptl_set_error_correction_scheme(struct ast_udptl *udptl, enum ast_t38_ec_modes ec);

void ast_udptl_set_local_max_ifp(struct ast_udptl *udptl, unsigned int max_ifp);

/*! 
 * \brief retrieves local_max_datagram.
 * 
 * \retval positive value representing max datagram size.
 * \retval 0 if no value is present
 */
unsigned int ast_udptl_get_local_max_datagram(struct ast_udptl *udptl);

/*! 
 * \brief sets far max datagram size.  If max_datagram is = 0, the far max datagram
 *  size is set to a default value.
 */
void ast_udptl_set_far_max_datagram(struct ast_udptl *udptl, unsigned int max_datagram);

unsigned int ast_udptl_get_far_max_datagram(const struct ast_udptl *udptl);

/*! 
 * \brief retrieves far max ifp
 * 
 * \retval positive value representing max ifp size
 * \retval 0 if no value is present
 */
unsigned int ast_udptl_get_far_max_ifp(struct ast_udptl *udptl);

void ast_udptl_setnat(struct ast_udptl *udptl, int nat);

void ast_udptl_stop(struct ast_udptl *udptl);

void ast_udptl_init(void);

/*!
 * \version 1.6.1 return changed to int
 */
int ast_udptl_reload(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
