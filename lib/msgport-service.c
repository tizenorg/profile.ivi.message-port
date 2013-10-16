#include "msgport-service.h"
#include "msgport-utils.h"
#include "common/dbus-service-glue.h"
#include "common/log.h"
#include <bundle.h>


struct _MsgPortService
{
    GObject parent;

    MsgPortDbusGlueService *proxy;
    guint                   on_messge_signal_id;
    messageport_message_cb  client_cb;
};

G_DEFINE_TYPE(MsgPortService, msgport_service, G_TYPE_OBJECT)

static void
_service_dispose (GObject *self)
{
    MsgPortService *service = MSGPORT_SERVICE (self);

    g_clear_object (&service->proxy);

    G_OBJECT_CLASS(msgport_service_parent_class)->dispose (self);
}

static void
msgport_service_class_init (MsgPortServiceClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS(klass);

    g_klass->dispose = _service_dispose;
}

static void
msgport_service_init (MsgPortService *service)
{
    service->proxy = NULL;
    service->client_cb = NULL;
    service->on_messge_signal_id = 0;
}

static void
_on_got_message (MsgPortService *service, GVariant *data, const gchar *remote_app_id, const gchar *remote_port, gboolean remote_is_trusted, gpointer userdata)
{
#ifdef ENABLE_DEBUG
    gchar *str_data = g_variant_print (data, TRUE);
    DBG ("Message received : %s(%"G_GSIZE_FORMAT")", str_data, g_variant_n_children (data));
    g_free (str_data);
#endif
    bundle *b = bundle_from_variant_map (data);

    service->client_cb (msgport_dbus_glue_service_get_id (service->proxy), remote_app_id, remote_port, remote_is_trusted, b);

    bundle_free (b);
}

MsgPortService *
msgport_service_new (GDBusConnection *connection, const gchar *path, messageport_message_cb message_cb)
{
    GError *error = NULL;

    MsgPortService *service = g_object_new (MSGPORT_TYPE_SERVICE, NULL);
    if (!service) {
        return NULL;
    }

    service->proxy = msgport_dbus_glue_service_proxy_new_sync (connection,
                G_DBUS_PROXY_FLAGS_NONE, NULL, path, NULL, &error);
    if (!service->proxy) {
        g_object_unref (service);
        WARN ("failed create servie proxy for path '%s' : %s", path, error->message);
        g_error_free (error);
        return NULL;
    }

    service->client_cb = message_cb;
    service->on_messge_signal_id = g_signal_connect_swapped (service->proxy, "on-message", G_CALLBACK (_on_got_message), service);

    return service;
}

guint
msgport_service_id (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), 0);
    g_return_val_if_fail (service->proxy && MSGPORT_DBUS_GLUE_IS_SERVICE (service->proxy), 0);

    return msgport_dbus_glue_service_get_id (service->proxy);
}

const gchar *
msgport_service_name (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), NULL);
    g_return_val_if_fail (service->proxy && MSGPORT_DBUS_GLUE_IS_SERVICE (service->proxy), NULL);

    return msgport_dbus_glue_service_get_port_name (service->proxy);
}

gboolean
msgport_service_is_trusted (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), FALSE);
    g_return_val_if_fail (service->proxy && MSGPORT_DBUS_GLUE_IS_SERVICE (service->proxy), FALSE);

    return msgport_dbus_glue_service_get_is_trusted (service->proxy);
}

gboolean
msgport_service_unregister (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), FALSE);
    g_return_val_if_fail (service->proxy, FALSE);

    /* fire and forget */
    return msgport_dbus_glue_service_call_unregister_sync (service->proxy, NULL, NULL);
}

messageport_error_e
msgport_service_send_message (MsgPortService *service, guint remote_service_id, GVariant *message)
{
    GError *error = NULL;
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (service->proxy, MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (message, MESSAGEPORT_ERROR_INVALID_PARAMETER);

    msgport_dbus_glue_service_call_send_message_sync (service->proxy, remote_service_id, message, NULL, &error);

    if (error) {
        messageport_error_e err = msgport_daemon_error_to_error (error);
        WARN ("Fail to send message on service %p to %d : %s", service, remote_service_id, error->message);
        g_error_free (error);
        return err;
    }

    return MESSAGEPORT_ERROR_NONE;
}

