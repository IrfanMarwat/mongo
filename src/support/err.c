/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2008-2011 WiredTiger, Inc.
 *	All rights reserved.
 */

#include "wt_internal.h"

/*
 * __wt_msg_call --
 *	Pass a message to a callback function.
 */
void
__wt_msgv(SESSION *session, const char *prefix1, const char *prefix2,
    const char *fmt, va_list ap)
{
	WT_EVENT_HANDLER *handler;
	char *end, *p;
	/*
	 * !!!
	 * SECURITY:
	 * Buffer placed at the end of the stack in case snprintf overflows.
	 */
	char s[2048];

	p = s;
	end = s + sizeof(s);

	if (prefix1 != NULL && prefix2 != NULL && p < end)
		p += snprintf(p, (size_t)(end - p),
		    "%s [%s]: ", prefix1, prefix2);
	else if (prefix1 != NULL && p < end)
		p += snprintf(p, (size_t)(end - p), "%s: ", prefix1);
	else if (prefix2 != NULL && p < end)
		p += snprintf(p, (size_t)(end - p), "%s: ", prefix2);
	if (p < end)
		p += vsnprintf(p, (size_t)(end - p), fmt, ap);

	handler = session->event_handler;
	(void)handler->handle_message(handler, s);
}

/*
 * __wt_msg --
 * 	Report a message.
 */
void
__wt_msg(SESSION *session, const char *fmt, ...)
    WT_GCC_ATTRIBUTE ((format (printf, 2, 3)))
{
	va_list ap;

	va_start(ap, fmt);
	__wt_msgv(session,
	    (session->btree != NULL) ? session->btree->name : NULL,
	    session->name, fmt, ap);
	va_end(ap);
}

#ifdef HAVE_DIAGNOSTIC
/*
 * __wt_assert --
 *	Internal version of assert function.
 */
void
__wt_assert(
    SESSION *session, const char *check, const char *file_name, int line_number)
{
	__wt_errx(session,
	    "assertion failure: %s/%d: \"%s\"", file_name, line_number, check);

	__wt_abort(session);
	/* NOTREACHED */
}
#endif

/*
 * __wt_file_format --
 *	Print a standard error message when a file format error is suddenly
 *	discovered.
 */
int
__wt_file_format(SESSION *session)
{
	__wt_errx(session, "the file is corrupted; use the Db.salvage"
	    " method or the db_salvage utility to repair the file");
	return (WT_ERROR);
}

/*
 * __wt_file_item_too_big --
 *	Print a standard error message when an element is too large to store.
 */
int
__wt_file_item_too_big(SESSION *session)
{
	__wt_errx(session, "the item is too large for the file to store");
	return (WT_ERROR);
}

/*
 * __wt_errv --
 * 	Report an error (va_list version).
 */
void
__wt_errv(SESSION *session, int error,
    const char *prefix1, const char *prefix2, const char *fmt, va_list ap)
{
	WT_EVENT_HANDLER *handler;
	char *end, *p;

	/*
	 * !!!
	 * SECURITY:
	 * Buffer placed at the end of the stack in case snprintf overflows.
	 */
	char s[2048];

	p = s;
	end = s + sizeof(s);

	if (prefix1 != NULL && prefix2 != NULL && p < end)
		p += snprintf(p, (size_t)(end - p),
		    "%s [%s]: ", prefix1, prefix2);
	else if (prefix1 != NULL && p < end)
		p += snprintf(p, (size_t)(end - p), "%s: ", prefix1);
	else if (prefix2 != NULL && p < end)
		p += snprintf(p, (size_t)(end - p), "%s: ", prefix2);
	if (p < end)
		p += vsnprintf(p, (size_t)(end - p), fmt, ap);
	if (error != 0 && p < end)
		p += snprintf(p,
		    (size_t)(end - p), ": %s", wiredtiger_strerror(error));

	handler = session->event_handler;
	handler->handle_error(handler, error, s);
}

/*
 * __wt_err --
 * 	Report an error.
 */
void
__wt_err(SESSION *session, int error, const char *fmt, ...)
    WT_GCC_ATTRIBUTE ((format (printf, 3, 4)))
{
	va_list ap;

	va_start(ap, fmt);
	__wt_errv(session, error,
	    (session->btree != NULL) ? session->btree->name : NULL,
	    session->name,
	    fmt, ap);
	va_end(ap);
}

/*
 * __wt_errx --
 * 	Report an error with no error code.
 */
void
__wt_errx(SESSION *session, const char *fmt, ...)
    WT_GCC_ATTRIBUTE ((format (printf, 2, 3)))
{
	va_list ap;

	va_start(ap, fmt);
	__wt_errv(session, 0,
	    (session->btree != NULL) ? session->btree->name : NULL,
	    session->name,
	    fmt, ap);
	va_end(ap);
}
