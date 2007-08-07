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

#ifndef _ASTERISK_MANAGER_H
#define _ASTERISK_MANAGER_H

#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "asterisk/lock.h"

/*!
 \file
 \brief The AMI - Asterisk Manager Interface - is a TCP protocol created to
 manage Asterisk with third-party software.

 Manager protocol packages are text fields of the form a: b.  There is
 always exactly one space after the colon.

\verbatim

 The first header type is the "Event" header.  Other headers vary from
 event to event.  Headers end with standard \r\n termination.
 The last line of the manager response or event is an empty line.
 (\r\n)

\endverbatim

 ** Please try to re-use existing headers to simplify manager message parsing in clients.
    Don't re-use an existing header with a new meaning, please.
    You can find a reference of standard headers in doc/manager.txt
 */

#define AMI_VERSION			"1.0"
#define DEFAULT_MANAGER_PORT 5038	/* Default port for Asterisk management via TCP */

#define EVENT_FLAG_SYSTEM 		(1 << 0) /* System events such as module load/unload */
#define EVENT_FLAG_CALL			(1 << 1) /* Call event, such as state change, etc */
#define EVENT_FLAG_LOG			(1 << 2) /* Log events */
#define EVENT_FLAG_VERBOSE		(1 << 3) /* Verbose messages */
#define EVENT_FLAG_COMMAND		(1 << 4) /* Ability to read/set commands */
#define EVENT_FLAG_AGENT		(1 << 5) /* Ability to read/set agent info */
#define EVENT_FLAG_USER                 (1 << 6) /* Ability to read/set user info */
#define EVENT_FLAG_CONFIG		(1 << 7) /* Ability to modify configurations */
#define EVENT_FLAG_DTMF  		(1 << 8) /* Ability to read DTMF events */
#define EVENT_FLAG_REPORTING		(1 << 9) /* Reporting events such as rtcp sent */
/* Export manager structures */
#define AST_MAX_MANHEADERS 128

/* Manager Helper Function */
typedef int (*manager_hook_t)(int, const char *, char *); 


struct manager_custom_hook {
	/*! Identifier */
	char *file;
	/*! helper function */
	manager_hook_t helper;
	/*! Linked list information */
	AST_RWLIST_ENTRY(manager_custom_hook) list;
};

/*! \brief Check if AMI is enabled */
int check_manager_enabled(void);

/*! \brief Check if AMI/HTTP is enabled */
int check_webmanager_enabled(void);

/*! Add a custom hook to be called when an event is fired */
/*! \param hook struct manager_custom_hook object to add
*/
void ast_manager_register_hook(struct manager_custom_hook *hook);

/*! Delete a custom hook to be called when an event is fired */
/*! \param hook struct manager_custom_hook object to delete
*/
void ast_manager_unregister_hook(struct manager_custom_hook *hook);

struct mansession;

struct message {
	unsigned int hdrcount;
	const char *headers[AST_MAX_MANHEADERS];
};

struct manager_action {
	/*! Name of the action */
	const char *action;
	/*! Short description of the action */
	const char *synopsis;
	/*! Detailed description of the action */
	const char *description;
	/*! Permission required for action.  EVENT_FLAG_* */
	int authority;
	/*! Function to be called */
	int (*func)(struct mansession *s, const struct message *m);
	/*! For easy linking */
	AST_RWLIST_ENTRY(manager_action) list;
};

/* External routines may register/unregister manager callbacks this way */
#define ast_manager_register(a, b, c, d) ast_manager_register2(a, b, c, d, NULL)

/* Use ast_manager_register2 to register with help text for new manager commands */

/*! Register a manager command with the manager interface */
/*! 	\param action Name of the requested Action:
	\param authority Required authority for this command
	\param func Function to call for this command
	\param synopsis Help text (one line, up to 30 chars) for CLI manager show commands
	\param description Help text, several lines
*/
int ast_manager_register2(
	const char *action,
	int authority,
	int (*func)(struct mansession *s, const struct message *m),
	const char *synopsis,
	const char *description);

/*! Unregister a registred manager command */
/*!	\param action Name of registred Action:
*/
int ast_manager_unregister( char *action );

/*! 
 * \brief Verify a session's read permissions against a permission mask.  
 * \param ident session identity
 * \param perm permission mask to verify
 * \retval 1 if the session has the permission mask capabilities
 * \retval 0 otherwise
 */
int astman_verify_session_readpermissions(unsigned long ident, int perm);

/*!
 * \brief Verify a session's write permissions against a permission mask.  
 * \param ident session identity
 * \param perm permission mask to verify
 * \retval 1 if the session has the permission mask capabilities, otherwise 0
 * \retval 0 otherwise
 */
int astman_verify_session_writepermissions(unsigned long ident, int perm);

/*! External routines may send asterisk manager events this way */
/*! 	\param category	Event category, matches manager authorization
	\param event	Event name
	\param contents	Contents of event
*/

/* XXX the parser in gcc 2.95 gets confused if you don't put a space
 * between the last arg before VA_ARGS and the comma */
#define manager_event(category, event, contents , ...)	\
        __manager_event(category, event, __FILE__, __LINE__, __PRETTY_FUNCTION__, contents , ## __VA_ARGS__)

int __attribute__ ((format(printf, 6, 7))) __manager_event(int category, const char *event,
							   const char *file, int line, const char *func,
							   const char *contents, ...);

/*! Get header from mananger transaction */
const char *astman_get_header(const struct message *m, char *var);

/*! Get a linked list of the Variable: headers */
struct ast_variable *astman_get_variables(const struct message *m);

/*! Send error in manager transaction */
void astman_send_error(struct mansession *s, const struct message *m, char *error);
void astman_send_response(struct mansession *s, const struct message *m, char *resp, char *msg);
void astman_send_ack(struct mansession *s, const struct message *m, char *msg);
void astman_send_listack(struct mansession *s, const struct message *m, char *msg, char *listflag);

void __attribute__ ((format (printf, 2, 3))) astman_append(struct mansession *s, const char *fmt, ...);

/*! Called by Asterisk initialization */
int init_manager(void);
int reload_manager(void);

#endif /* _ASTERISK_MANAGER_H */
