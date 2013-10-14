#ifndef __MSGPORT_ERROR_H
#define __MSGPORT_ERROR_H

#include <glib.h>

#define MSGPORT_ERROR_QUARK (msgport_error_quark())

G_BEGIN_DECLS

typedef enum _MsgPortError
{
    MSGPORT_ERROR_IO_ERROR = 100,
    MSGPORT_ERROR_OUT_OF_MEMORY,
    MSGPORT_ERROR_INVALID_PARAMS,
    MSGPORT_ERROR_NOT_FOUND,
    MSGPORT_ERROR_ALREADY_EXISTING,
    MSGPORT_ERROR_CERTIFICATE_MISMATCH,
    MSGPORT_ERROR_UNKNOWN

} MsgPortError;

GQuark
msgport_error_quark ();

#define msgport_error_new(id, msg, args...) \
    g_error_new (MSGPORT_ERROR_QUARK, id, msg, ##args) 

#define msgport_error_port_existing_new(app_id, port_name) \
    msgport_error_new (MSGPORT_ERROR_ALREADY_EXISTING, \
                       "port already existing with name '%s' on application '%s'", port_name, app_id)

#define msgport_error_no_memory_new() \
    msgport_error_new (MSGPORT_ERROR_OUT_OF_MEMORY, "no memomry")

#define msgport_error_port_not_found(app_id, port_name) \
    msgport_error_new (MSGPORT_ERROR_NOT_FOUND, "port not found with name '%s' on application '%s'", port_name, app_id)

#define msgport_error_port_id_not_found_new(service_id) \
    msgport_error_new (MSGPORT_ERROR_NOT_FOUND, "no port found with id '%d'", service_id);

#define msgport_error_unknown_new() \
    msgport_error_new (MSGPORT_ERROR_UNKNOWN, "unknown");

G_END_DECLS

#endif /* __MSGPORT_ERROR_H */
