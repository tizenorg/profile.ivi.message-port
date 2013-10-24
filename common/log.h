#ifndef _MSGPORT_LOG_H
#define _MSGPORT_LOG_H

#include <glib.h>
#include "config.h"

#ifdef HAVE_DLOG

#   include <dlog.h>

#   define __LOG(prio, frmt, args...) print_log(prio, LOG_TAG, "%s +%d:"frmt, __FILE__, __LINE__, ##args)

#   define DBG(frmt, args...) __LOG(DLOG_DEBUG, frmt, ##args)
#   define WARN(frmt, args...) __LOG(DLOG_WARN, frmt, ##args)
#   define ERR(frmt, args...) __LOG(DLOG_ERROR, frmt, ##args)

#else /* USE_DLOG */

#   define __LOG(log_func, frmt, args...) log_func("%s +%d:"frmt, __FILE__, __LINE__, ##args)

#   define DBG(frmt, args...)  __LOG(g_debug, frmt, ##args)
#   define WARN(frmt, args...) __LOG(g_warning, frmt, ##args)
#   define ERR(frmt, args...)  __LOG(g_error, frmt, ##args)

#endif /* USE_DLOG */

#endif /* _MSGPORT_LOG_H */
