#include "msgport-manager.h"
#include "message-port.h"
#include "common/log.h"
#include "dbus-manager-glue.h"
#include <gio/gio.h>

struct _MsgPortManager
{
    MsgPortGlueManagerPorxy *proxy;
    GHashTable *services; /* {gchar*:MsgPortService*} */
    GList *local_service_list;
    int n_local_services;
};

static MsgPortManager __manager;

MsgPortManager * msgport_manager_new ()
{
    GError          *error = NULL;
    GDBusConnection *connection = NULL;
    MsgPortManager  *manager = g_slice_new0(MsgPortManager);
    gchar           *bus_address = g_strdup_printf (MESSAGEPORT_BUS_ADDRESS, g_get_user_runtime_dir());

    if (!manager) {
        return NULL;
    }

    connection = g_dbus_connection_new_for_address_sync (bus_address,
                                G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
                                NULL, NULL, &error);

    if (error) {
        ERR ("Unable to get bus connection for address %s: %s", bus_address, error->message);
        g_error_free (error);
        goto fail;
    }

    manager->services = g_hash_table_new_full (g_str_hash, g_str_equal,
                               g_free, (GDestroyNotify)g_object_unref);
    manager->local_servcie_list = NULL;
    manager->n_local_services = 0;
    manager->proxy = msgport_dbus_glue_manager_proxy_new_sync (
            connection, G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES, NULL, "/", NULL, &error);

    if (!manager->proxy) {
        ERR ("Unable to get manager proxy : %s", error->message);
        g_error_free (error);
        goto fail;
    }

    return manager;

fail:
    if (manager) msgport_manager_unref (manager);
    return NULL;
}

MsgPortManager * msgport_get_manager () {
    if (!__manager) 
        __manager = magport_manager_new ();

    return __manager;
}

int msgport_manager_register_service (MsgPortManager *manger, const gchar *port_name, gboolean is_trusted)
{
    GError *error = NULL;
    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), MESSAGEPORT_ERROR_IO_ERROR);

    if (!port_name) return MESSAGEPORT_ERROR_INVALID_PARAMETER;

    msgport_dbus_glue_manager_call_register_service_sync (manager->proxy,
            port_name, is_trusted, &object_path, NULL, &error);

    if (error) {
        WARN ("unable to register service (%s): %s", port_name, error->message);
        g_error_free (error);
        return MESSAGEPORT_ERROR_IO_ERROR;
    }

    MsgPortService *service = msgport_service_new (
            g_dbus_proxy_get_connection (G_DBUS_PROXY(manager->proxy)),
            object_path, &error);
    if (!service) {
        WARN ("unable to get service proxy for path %s: %s", object_path, error->message);
        g_error_free (error);
        g_free (object_path);
        return MESSAGEPORT_ERROR_IO_ERROR;
    }

    g_hash_table_insert (manager->services, object_path, service);
    manager->local_service_list = g_list_append (manager->local_service_list, object_path);
    manager->n_local_services++;

    return manager->n_local_services;
}

MsgPortService *
msgport_service_new (GDBusConnection *connection, const gchar *path, GError **error)
{
    MsgPortService *service = g_object_new (MSGPORT_TYPE_SERVICE, NULL);

    if (!service) {
        /* FIXME: return no merory error */
        return NULL
    }

    service->proxy = msgport_dbus_glue_service_proxy_new_sync (connection,
                g_dbus_proxy_get_connection (G_DBUS_PROXY(manager->proxy)),
                G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES, NULL, object_path, NULL, error);
    if (!service->proxy) {
        g_object_unref (service);
        return NULL;
    }

    return service;
}

