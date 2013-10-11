#ifndef _MSGPORT_LOG_H
#define _MSGPORT_LOG_H

#include <glib.h>

#define LOG(log_func, frmt, args...) log_func("%s +%d:"frmt, __FILE__, __LINE__, ##args)

#define DBG(frmt, args...)  LOG(g_debug, frmt, ##args)
#define WARN(frmt, args...) LOG(g_warning, frmt, ##args)
#define ERR(frmt, args...)  LOG(g_error, frmt, ##args)

#endif /* _MSGPORT_LOG_H */
