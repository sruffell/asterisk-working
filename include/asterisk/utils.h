/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Utility functions
 *
 * Copyright (C) 2004, Digium
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */

#ifndef _ASTERISK_UTIL_H
#define _ASTERISK_UTIL_H

#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <asterisk/lock.h>
#include <limits.h>

/* Note:
   It is very important to use only unsigned variables to hold
   bit flags, as otherwise you can fall prey to the compiler's
   sign-extension antics if you try to use the top two bits in
   your variable.

   The flag macros below use a set of compiler tricks to verify
   that the caller is using an "unsigned int" variable to hold
   the flags, and nothing else. If the caller uses any other
   type of variable, a warning message similar to this:

   warning: comparison of distinct pointer types lacks cast

   will be generated.

   The "dummy" variable below is used to make these comparisons.

   Also note that at -O2 or above, this type-safety checking
   does _not_ produce any additional object code at all.
*/

extern unsigned int __unsigned_int_flags_dummy;

#define ast_test_flag(p,flag) 		({ \
					typeof ((p)->flags) __p = (p)->flags; \
					typeof (__unsigned_int_flags_dummy) __x = 0; \
					(void) (&__p == &__x); \
					((p)->flags & (flag)); \
					})

#define ast_set_flag(p,flag) 		do { \
					typeof ((p)->flags) __p = (p)->flags; \
					typeof (__unsigned_int_flags_dummy) __x = 0; \
					(void) (&__p == &__x); \
					((p)->flags |= (flag)); \
					} while(0)

#define ast_clear_flag(p,flag) 		do { \
					typeof ((p)->flags) __p = (p)->flags; \
					typeof (__unsigned_int_flags_dummy) __x = 0; \
					(void) (&__p == &__x); \
					((p)->flags &= ~(flag)); \
					} while(0)

#define ast_copy_flags(dest,src,flagz)	do { \
					typeof ((dest)->flags) __d = (dest)->flags; \
					typeof ((src)->flags) __s = (src)->flags; \
					typeof (__unsigned_int_flags_dummy) __x = 0; \
					(void) (&__d == &__x); \
					(void) (&__s == &__x); \
					(dest)->flags &= ~(flagz); \
					(dest)->flags |= ((src)->flags & (flagz)); \
					} while (0)

#define ast_set2_flag(p,value,flag)	do { \
					typeof ((p)->flags) __p = (p)->flags; \
					typeof (__unsigned_int_flags_dummy) __x = 0; \
					(void) (&__p == &__x); \
					if (value) \
						(p)->flags |= (flag); \
					else \
						(p)->flags &= ~(flag); \
					} while (0)

/* Non-type checking variations for non-unsigned int flags.  You
   should only use non-unsigned int flags where required by 
   protocol etc and if you know what you're doing :)  */
#define ast_test_flag_nonstd(p,flag) 		({ \
					((p)->flags & (flag)); \
					})

#define ast_set_flag_nonstd(p,flag) 		do { \
					((p)->flags |= (flag)); \
					} while(0)

#define ast_clear_flag_nonstd(p,flag) 		do { \
					((p)->flags &= ~(flag)); \
					} while(0)

#define ast_copy_flags_nonstd(dest,src,flagz)	do { \
					(dest)->flags &= ~(flagz); \
					(dest)->flags |= ((src)->flags & (flagz)); \
					} while (0)

#define ast_set2_flag_nonstd(p,value,flag)	do { \
					if (value) \
						(p)->flags |= (flag); \
					else \
						(p)->flags &= ~(flag); \
					} while (0)

#define AST_FLAGS_ALL UINT_MAX

struct ast_flags {
	unsigned int flags;
};

static inline int ast_strlen_zero(const char *s)
{
	return (*s == '\0');
}

struct ast_hostent {
	struct hostent hp;
	char buf[1024];
};

extern char *ast_strip(char *buf);
extern struct hostent *ast_gethostbyname(const char *host, struct ast_hostent *hp);
extern int ast_base64encode(char *dst, unsigned char *src, int srclen, int max);
extern int ast_base64decode(unsigned char *dst, char *src, int max);

extern int test_for_thread_safety(void);
extern const char *ast_inet_ntoa(char *buf, int bufsiz, struct in_addr ia);
extern int ast_utils_init(void);

#ifdef inet_ntoa
#undef inet_ntoa
#endif
#define inet_ntoa __dont__use__inet_ntoa__use__ast_inet_ntoa__instead__

#ifdef __linux__
#define ast_pthread_create pthread_create
#define ast_strcasestr strcasestr
#else
/* Linux threads have a default 2MB stack size. */
#ifndef PTHREAD_ATTR_STACKSIZE
#define	PTHREAD_ATTR_STACKSIZE		2097152
#endif /* PTHREAD_ATTR_STACKSIZE */
extern int ast_pthread_create(pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *), void *data);
#endif /* __linux__ */

extern char *ast_strcasestr(const char *, const char *);

#endif
