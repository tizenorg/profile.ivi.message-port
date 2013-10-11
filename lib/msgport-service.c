#include "msgport-service.h"
#include "msgport-utils.h"
#include "common/dbus-service-glue.h"
#include "common/log.h"
#include <bundle.h>


struct _MsgPortService
{
    GObject parent;

    MsgPortDbusGlueService *proxy;

    int id;              /* unique service id */
    gchar *name;         /* service name */
    gboolean is_trusted; /* is trusted service */

    guint on_messge_signal_id;
    messageport_message_cb client_cb;
};

static int __object_counter;

G_DEFINE_TYPE(MsgPortService, msgport_service, G_TYPE_OBJECT)

static void
_service_finalize (GObject *self)
{
    MsgPortService *service = MSGPORT_SERVICE (self);

    g_free (service->name);

    G_OBJECT_CLASS (msgport_service_parent_class)->finalize (self);
}

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
    g_klass->finalize = _service_finalize;
}

static void
msgport_service_init (MsgPortService *service)
{
    service->proxy = NULL;
    service->id = 0;
    service->name = NULL;
    service->is_trusted = FALSE;
    service->client_cb = NULL;
    service->on_messge_signal_id = 0;
}

static void
_on_got_message (MsgPortService *service, GVariant *data, const gchar *remote_app_id, const gchar *remote_port, gboolean remote_is_trusted, gpointer userdata)
{
    gchar *str_data = g_variant_print (data, TRUE);
    DBG ("Message received : %s(%d)", str_data, g_variant_n_children (data));
    g_free (str_data);
    bundle *b = bundle_from_variant_map (data);

    service->client_cb (service->id, remote_app_id, remote_port, remote_is_trusted, b);

    bundle_free (b);
}

MsgPortService *
msgport_service_new (GDBusConnection *connection, const gchar *path, messageport_message_cb message_cb)
{
    GVariant *properties = NULL;
    GVariantIter iter;
    GError *error = NULL;
    MsgPortService *service = g_object_new (MSGPORT_TYPE_SERVICE, NULL);

    if (!service) {
        /* FIXME: return no merory error */
        return NULL;
    }

    service->proxy = msgport_dbus_glue_service_proxy_new_sync (connection,
                G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                NULL, path, NULL, &error);
    if (!service->proxy) {
        g_object_unref (service);
        WARN ("failed create servie proxy for path '%s' : %s", path,
                error->message);
        g_error_free (error);
        return NULL;
    }

    service->id = ++__object_counter;

    if (msgport_dbus_glue_service_call_get_properties_sync (service->proxy, &properties, NULL, &error)) {
        gchar *key = NULL;
        GVariant *value = NULL;
        g_variant_iter_init (&iter, properties);

        while (g_variant_iter_next (&iter, "{sv}", &key, &value)) {
            if ( !g_strcmp0 (key, "PortName")) service->name = g_strdup (g_variant_get_string (value, NULL));
            else if ( !g_strcmp0 (key, "IsTrusted")) service->is_trusted = g_variant_get_boolean (value);
            else {
                WARN ("Unknown property : %s", key);
            }

            g_free (key);
            g_variant_unref (value);
        }
    }
    else {
        WARN ("Failed to get properties from service proxy '%s' : %s",
                path, error->message);
        g_error_free (error);
    }

    service->client_cb = message_cb;
    service->on_messge_signal_id = g_signal_connect_swapped (service->proxy, "on-message", G_CALLBACK (_on_got_message), service);

    return service;
}


const gchar *
msgport_service_name (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), NULL);

    return service->name;
}

gboolean
msgport_service_is_trusted (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), FALSE);

    return service->is_trusted;
}

int
msgport_service_id (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), 0);

    return service->id;
}

gboolean
msgport_service_unregister (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), FALSE);

    return TRUE;
}

