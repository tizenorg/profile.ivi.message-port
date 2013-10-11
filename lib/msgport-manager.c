#include "msgport-manager.h"
#include "msgport-service.h"
#include "message-port.h" /* MESSAGEPORT_ERROR */
#include "common/dbus-manager-glue.h"
#include "common/log.h"
#include "config.h" /* MESSAGEPORT_BUS_ADDRESS */
#include <gio/gio.h>

struct _MsgPortManager
{
    GObject parent;

    MsgPortDbusGlueManager *proxy;
    GHashTable *services; /* {gchar*:MsgPortService*} */
    GHashTable *local_services; /* {gint: gchar *} */ 
    GHashTable *remote_services; /* {gint: gchar *} */
};

G_DEFINE_TYPE (MsgPortManager, msgport_manager, G_TYPE_OBJECT)

static void
_unregister_service_cb (int service_id, const gchar *object_path, MsgPortManager *manager)
{
    MsgPortService *service = g_hash_table_lookup (manager->services, object_path);

    if (service) msgport_service_unregister (service);
}

static void
_finalize (GObject *self)
{
    MsgPortManager *manager = MSGPORT_MANAGER (self);

    if (manager->local_services) {
        g_hash_table_unref (manager->local_services);
        manager->local_services = NULL;
    }

    if (manager->remote_services) {
        g_hash_table_unref (manager->remote_services);
        manager->remote_services = NULL;
    }

    G_OBJECT_CLASS (msgport_manager_parent_class)->finalize (self);
}

static void
_dispose (GObject *self)
{
    MsgPortManager *manager = MSGPORT_MANAGER (self);

    g_hash_table_foreach (manager->local_services, (GHFunc)_unregister_service_cb, manager);

    if (manager->services) {
        g_hash_table_unref (manager->services);
        manager->services = NULL;
    }

    g_clear_object (&manager->proxy);

    G_OBJECT_CLASS (msgport_manager_parent_class)->dispose (self);
}

static void
msgport_manager_class_init (MsgPortManagerClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = _finalize;
    g_klass->dispose = _dispose;
}

static void
msgport_manager_init (MsgPortManager *manager)
{
    GError          *error = NULL;
    GDBusConnection *connection = NULL;
    gchar           *bus_address = g_strdup_printf (MESSAGEPORT_BUS_ADDRESS);

    manager->services = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
    manager->local_services = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);
    manager->remote_services = g_hash_table_new_full (g_direct_hash, g_direct_equal, NULL, NULL);

    connection = g_dbus_connection_new_for_address_sync (bus_address,
            G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT, NULL, NULL, &error);
    if (error) {
        WARN ("Fail to connect messageport server at address %s: %s", bus_address, error->message);
        g_error_free (error);
    }
    else {
        manager->proxy = msgport_dbus_glue_manager_proxy_new_sync (
            connection, G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES, NULL, "/", NULL, &error);
        if (error) {
            WARN ("Fail to get manager proxy : %s", error->message);
            g_error_free (error);
        }
    }
}

MsgPortManager * msgport_manager_new ()
{
    return g_object_new (MSGPORT_TYPE_MANAGER, NULL);
}

static MsgPortManager *__manager;

MsgPortManager * msgport_get_manager () 
{
    if (!__manager) {
        __manager = msgport_manager_new ();
    }

    return __manager;
}

static int
_create_and_cache_service (MsgPortManager *manager, gchar *object_path, messageport_message_cb cb)
{
    int id;
    MsgPortService *service = msgport_service_new (
            g_dbus_proxy_get_connection (G_DBUS_PROXY(manager->proxy)),
            object_path, cb);
    if (!service) {
        return MESSAGEPORT_ERROR_IO_ERROR;
    }

    id = msgport_service_id (service);

    g_hash_table_insert (manager->services, object_path, service);
    g_hash_table_insert (manager->local_services, GINT_TO_POINTER (id), object_path);

    return id;
}

static MsgPortService *
_get_local_port (MsgPortManager *manager, int service_id)
{
    const gchar *object_path = NULL;
    MsgPortService *service = NULL;

    object_path = g_hash_table_lookup (manager->local_services, GINT_TO_POINTER(service_id));
    if (!object_path) return NULL;

    service = MSGPORT_SERVICE (g_hash_table_lookup (manager->services, object_path));
    if (!service) {
        g_hash_table_remove (manager->local_services, GINT_TO_POINTER (service_id));
        return NULL;
    }

    return service;
}

messageport_error_e
msgport_manager_register_service (MsgPortManager *manager, const gchar *port_name, gboolean is_trusted, messageport_message_cb message_cb, int *service_id)
{
    GError *error = NULL;
    gchar *object_path = NULL;

    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (manager->proxy, MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (service_id && port_name && message_cb, MESSAGEPORT_ERROR_INVALID_PARAMETER);

    msgport_dbus_glue_manager_call_register_service_sync (manager->proxy,
            port_name, is_trusted, &object_path, NULL, &error);

    if (error) {
        WARN ("unable to register service (%s): %s", port_name, error->message);
        g_error_free (error);
        return MESSAGEPORT_ERROR_IO_ERROR;
    }

    *service_id = _create_and_cache_service (manager, object_path, message_cb);

    return MESSAGEPORT_ERROR_NONE;
}

messageport_error_e
msgport_manager_unregister_servcie (MsgPortManager *manager, int service_id)
{
    const gchar *object_path = NULL;
    MsgPortService *service = NULL;
    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), FALSE);
    g_return_val_if_fail (manager->proxy, MESSAGEPORT_ERROR_IO_ERROR);

    service = _get_local_port (manager, service_id);
    if (!service) {
        return MESSAGEPORT_ERROR_MESSAGEPORT_NOT_FOUND;
    }

    if (!msgport_service_unregister (service)) 
        return MESSAGEPORT_ERROR_IO_ERROR;

    object_path = (const gchar *)g_hash_table_lookup (manager->local_services,
                                                      GINT_TO_POINTER(service_id));
    g_hash_table_remove (manager->local_services, GINT_TO_POINTER(service_id));
    g_hash_table_remove (manager->services, object_path);

    return MESSAGEPORT_ERROR_NONE;
}

messageport_error_e 
msgport_manager_check_remote_service (MsgPortManager *manager, const gchar *app_id, const gchar *port, gboolean is_trusted, guint *service_id_out)
{
    GError *error = NULL;
    guint remote_service_id = 0;

    if (service_id_out) *service_id_out = 0;

    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (manager->proxy, MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (app_id && port, MESSAGEPORT_ERROR_INVALID_PARAMETER);

    if (!app_id || !port) return MESSAGEPORT_ERROR_INVALID_PARAMETER;

    msgport_dbus_glue_manager_call_check_for_remote_service_sync (manager->proxy,
            app_id, port, is_trusted, &remote_service_id, NULL, &error);

    if (error) {
        WARN ("No service found for app_id %s, port name %s: %s", app_id, port, error->message);
        g_error_free (error);
        return MESSAGEPORT_ERROR_MESSAGEPORT_NOT_FOUND;
    }
    else {
        DBG ("Got service id %d for %s, %s", remote_service_id, app_id, port);

        if (service_id_out)  *service_id_out = remote_service_id;
    }

    return MESSAGEPORT_ERROR_NONE;
}

messageport_error_e
msgport_manager_get_service_name (MsgPortManager *manager, int service_id, gchar **name_out)
{
    MsgPortService *service = NULL;
    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (manager->proxy, MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (name_out && service_id, MESSAGEPORT_ERROR_INVALID_PARAMETER);

    service = _get_local_port (manager, service_id);
    if (!service) return MESSAGEPORT_ERROR_MESSAGEPORT_NOT_FOUND;

    *name_out = g_strdup (msgport_service_name (service));
    DBG ("PORT NAME : %s", *name_out);

    return MESSAGEPORT_ERROR_NONE;
}

messageport_error_e
msgport_manager_send_message (MsgPortManager *manager, const gchar *remote_app_id, const gchar *remote_port, gboolean is_trusted, GVariant *data)
{
    guint service_id = 0;
    messageport_error_e res = MESSAGEPORT_ERROR_NONE;
    GError *error = NULL;

    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (manager->proxy, MESSAGEPORT_ERROR_IO_ERROR);
    g_return_val_if_fail (remote_app_id && remote_port, MESSAGEPORT_ERROR_INVALID_PARAMETER);

    res = msgport_manager_check_remote_service (manager, remote_app_id, remote_port, is_trusted, &service_id);
    if (service_id == 0) return res;

    msgport_dbus_glue_manager_call_send_message_sync (manager->proxy, service_id, data, NULL, &error);

    if (error) {
        WARN ("Failed to send message to (%s:%s) : %s", remote_app_id, remote_port, error->message);
        g_error_free (error);
        res = MESSAGEPORT_ERROR_IO_ERROR;
    }

    return res;
}

