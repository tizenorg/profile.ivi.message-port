#ifndef __MSGPORT_UTILS_H
#define __MSGPORT_UTILS_H

#include <glib.h>
#include "utils.h"

#define msgport_return_if_fail(expr)      g_return_if_fail (expr)
#define msgport_return_if_fail_with_error(expr, err) \
do {\
    if (G_LIKELY (expr)) { }\
    else \
        g_return_if_fail_warning (G_LOG_DOMAIN,__PRETTY_FUNCTION__, ##expr);\
        if (err) *err = msgport_error_new (MSGPORT_ERROR_INVALID_PARAMS, "assert("#expr")"); \
        return; \
    }\
} while(0);

#define msgport_return_val_if_fail(expr, ret)     g_return_val_if_fail (expr, ret)
#define msgport_return_val_if_fail_with_error(expr, ret, err) \
do {\
    if (G_LIKELY(expr)) { } \
    else {\
        g_return_if_fail_warning (G_LOG_DOMAIN,__PRETTY_FUNCTION__, #expr);\
        if (err) *err = msgport_error_new (MSGPORT_ERROR_INVALID_PARAMS, "assert("#expr")"); \
        return ret; \
    }\
} while (0);

#endif /* __MSGPORT_UTILS_H */
