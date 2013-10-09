#include "message-port.h"

static int
_messageport_register_port (const char *name, gboolean is_trusted, messageport_message_cb cb)
{
    (void) name;
    (void) is_trusted;
    (void) cb;

    return MESSAGEPORT_ERROR_NONE;
}

static int
_messageport_check_remote_port (const char *app_id, const char *port, gboolean is_trusted, gboolean *exists)
{
    (void) app_id;
    (void) port;
    (void) is_trusted;

    if (exists) *exists = FALSE;

    return MESSAGEPORT_ERROR_MESSAGEPORT_NOT_FOUND;
}

static int
_messageport_send_message (const char *app_id, const char *port, gboolean is_trusted, bundle *message)
{
    (void) app_id;
    (void) port;
    (void) is_trusted;
    (void) message;

    return MESSAGEPORT_ERROR_NONE;
}

static int
_messageport_get_port_name (int port_id, gboolean is_trusted, gchar **name_out)
{
    (void) port_id;
    (void) is_trusted;

    if (name_out) **name_out = "";

    return MESSAGEPORT_ERROR_MESSAGEPORT_NOT_FOUND;
}

/*
 * API
 */

int messageport_register_local_port(const char* local_port, messageport_message_cb callback)
{
    return _messageport_register_port (local_port, FALSE, callback);
}

int messageport_register_trusted_local_port (const char *local_port, messageport_message_cb callback)
{
    return _messageport_register_port (local_port, TRUE, callback);
}

int messageport_check_remote_port (const char *remote_app_id, const char *port_name, gboolean *exists)
{
    return _messageport_check_remote_port (remote_app_id, port_name, FALSE, exists);
}

int messageport_check_trusted_remote_port (const char *remote_app_id, const char *port_name, gboolean *exists)
{
    return _messageport_check_remote_port (remote_app_id, port_name, TRUE, exists);
}

int messageport_send_message (const char* remote_app_id, const char* remote_port, bundle* message)
{
    return _messageport_send_message (remote_app_id, remote_port, FALSE, message);
}

int messageport_send_trusted_message(const char* remote_app_id, const char* remote_port, bundle* message)
{
    return _messageport_send_message (remote_app_id, remote_port, TRUE, message);
}

int messageport_send_bidirectional_message(int id, const char* remote_app_id, const char* remote_port, bundle* data)
{
    return _messageport_send_message (remote_app_id, remote_port, FALSE, data);
}

int messageport_send_bidirectional_trusted_message (int id, const char *remote_app_id, const char *remote_port, bundle *data)
{
    return _messageport_send_message (remote_app_id, remote_port, TRUE, data);
}

int messageport_get_local_port_name(int id, char **name)
{
    return _messageport_get_port_name (id, FALSE, name);
}

int messageport_check_trusted_local_port (int id, char **name)
{
    return _messageport_get_port_name (id, TRUE, name);
}

