#include "message-port.h"
#include "msgport-manager.h"
#include "msgport-utils.h"
#include "common/log.h"

static int
_messageport_register_port (const char *name, gboolean is_trusted, messageport_message_cb cb)
{
    int port_id = 0; /* id of the port created */
    messageport_error_e res;
    MsgPortManager *manager = msgport_get_manager ();

    g_assert (manager);
    g_assert (MSGPORT_IS_MANAGER (manager));

    res = msgport_manager_register_service (manager, name, is_trusted, cb, &port_id);

    return port_id > 0 ? port_id : (int)res;
}

static messageport_error_e
_messageport_check_remote_port (const char *app_id, const char *port, gboolean is_trusted, gboolean *exists)
{
    messageport_error_e res;
    MsgPortManager *manager = msgport_get_manager ();

    res = msgport_manager_check_remote_service (manager, app_id, port, FALSE, NULL);

    if (exists) *exists = (res == MESSAGEPORT_ERROR_NONE);

    return res;
}

static messageport_error_e
_messageport_send_message (const char *app_id, const char *port, gboolean is_trusted, bundle *message)
{
    MsgPortManager *manager = msgport_get_manager ();

    GVariant *v_data = bundle_to_variant_map (message);

    return msgport_manager_send_message (manager, app_id, port, is_trusted, v_data);
}

messageport_error_e
_messageport_send_bidirectional_message (int id, const gchar *remote_app_id, const gchar *remote_port, gboolean is_trusted, bundle *message)
{
    MsgPortManager *manager = msgport_get_manager ();

    GVariant *v_data = bundle_to_variant_map (message);

    return msgport_manager_send_bidirectional_message (manager, id, remote_app_id, remote_port, is_trusted, v_data);
}

/*
 * API
 */

int
messageport_register_local_port(const char* local_port, messageport_message_cb callback)
{
    return _messageport_register_port (local_port, FALSE, callback);
}

messageport_error_e
messageport_register_trusted_local_port (const char *local_port, messageport_message_cb callback)
{
    return _messageport_register_port (local_port, TRUE, callback);
}

messageport_error_e
messageport_check_remote_port (const char *remote_app_id, const char *port_name, gboolean *exists)
{
    return _messageport_check_remote_port (remote_app_id, port_name, FALSE, exists);
}

messageport_error_e
messageport_check_trusted_remote_port (const char *remote_app_id, const char *port_name, gboolean *exists)
{
    return _messageport_check_remote_port (remote_app_id, port_name, TRUE, exists);
}

messageport_error_e
messageport_send_message (const char* remote_app_id, const char* remote_port, bundle* message)
{
    return _messageport_send_message (remote_app_id, remote_port, FALSE, message);
}

messageport_error_e
messageport_send_trusted_message(const char* remote_app_id, const char* remote_port, bundle* message)
{
    return _messageport_send_message (remote_app_id, remote_port, TRUE, message);
}

messageport_error_e
messageport_send_bidirectional_message(int id, const char* remote_app_id, const char* remote_port, bundle* data)
{
    return _messageport_send_bidirectional_message (id, remote_app_id, remote_port, FALSE, data);
}

messageport_error_e
messageport_send_bidirectional_trusted_message (int id, const char *remote_app_id, const char *remote_port, bundle *data)
{
    return _messageport_send_bidirectional_message (id, remote_app_id, remote_port, TRUE, data);
}

messageport_error_e
messageport_get_local_port_name(int port_id, char **name_out)
{
    MsgPortManager *manager = msgport_get_manager ();

    return msgport_manager_get_service_name (manager, port_id, name_out);
}

messageport_error_e
messageport_check_trusted_local_port (int id, gboolean *trusted)
{
    (void) id;

    if (trusted) *trusted = FALSE;

    return MESSAGEPORT_ERROR_NONE;
}

