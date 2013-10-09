/* vi: set et sw=4 ts=4 cino=t0,(0: */
/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2013 Intel Corporation.
 *
 * Contact: Amarnath Valluri <amarnath.valluri@linux.intel.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "manager.h"
#include "common/log.h"
#include "dbus-manager.h"
#include "dbus-service.h"

G_DEFINE_TYPE (MsgPortManager, msgport_manager, G_TYPE_OBJECT)

#define MSGPORT_MANAGER_GET_PRIV(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE ((obj), MSGPORT_TYPE_MANAGER, MsgPortManagerPrivate)

struct _MsgPortManagerPrivate {
    /*
     * Key : const gchar * - object_path of the service
     * Value : MsgPortDbusService * (transfe full)
     */
    GHashTable *path_service_map; /* {object_path,MsgPortDbusService} */

    /*
     * Holds services owned by a client 
     * Key : MsgPortDbusManager *
     * Value : GList<MsgPortDbusService *> (tranfer none)
     */
    GHashTable *owner_service_map; /* {app_id,GList[MsgPortDbusService]} */
};

static void
_manager_finalize (GObject *self)
{
    //MsgPortManager *manager = MSGPORT_MANAGER (self);

    G_OBJECT_CLASS (msgport_manager_parent_class)->finalize (self);
}

static void
_manager_dispose (GObject *self)
{
    MsgPortManager *manager = MSGPORT_MANAGER (self);

    g_hash_table_unref (manager->priv->owner_service_map);
    manager->priv->owner_service_map = NULL;

    g_hash_table_unref (manager->priv->path_service_map);
    manager->priv->path_service_map = NULL;

    G_OBJECT_CLASS (msgport_manager_parent_class)->dispose (self);
}

static void
msgport_manager_init (MsgPortManager *self)
{
    MsgPortManagerPrivate *priv = MSGPORT_MANAGER_GET_PRIV (self);

    priv->path_service_map = g_hash_table_new_full (
                g_str_hash, g_str_equal, NULL, g_object_unref);
    priv->owner_service_map = g_hash_table_new_full (
                g_direct_hash, g_direct_equal, 
                NULL, (GDestroyNotify) g_list_free);

    self->priv = priv;
}

static void
msgport_manager_class_init (MsgPortManagerClass *klass)
{
    GObjectClass *gklass = G_OBJECT_CLASS(klass);

    g_type_class_add_private (klass, sizeof(MsgPortManagerPrivate));

    gklass->finalize = _manager_finalize;
    gklass->dispose = _manager_dispose;
}

MsgPortManager *
msgport_manager_new ()
{
    static GObject *manager = NULL;

    if (!manager) {
        manager =  g_object_new (MSGPORT_TYPE_MANAGER, NULL);
        g_object_add_weak_pointer (manager, (gpointer *)&manager);

        return MSGPORT_MANAGER (manager);
    }

    return MSGPORT_MANAGER (g_object_ref (manager));
}

MsgPortDbusService *
msgport_manager_register_service (
    MsgPortManager     *manager,
    MsgPortDbusManager *owner,
    const gchar        *port_name,
    gboolean            is_trusted,
    GError            **error)
{
    GList   *service_list  = NULL; /* services list by app_id */
    gboolean was_empty = TRUE;
    MsgPortDbusService *dbus_service = NULL;

    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), NULL);
    g_return_val_if_fail (owner && MSGPORT_IS_DBUS_MANAGER (owner), NULL);

    if ((service_list = (GList *)g_hash_table_lookup (manager->priv->owner_service_map, owner)) != NULL) {
        GList *list = NULL;

        for (list = service_list; list != NULL; list = list->next) {
            MsgPortDbusService *dbus_service = (MsgPortDbusService *)list->data;

            if ( !g_strcmp0 (port_name, msgport_dbus_service_get_port_name (dbus_service))) {
                /* FIXME: return EALREADY error */
                return NULL;
            }
        }
    }

    was_empty = (service_list == NULL);

    dbus_service = msgport_dbus_service_new (owner, port_name, is_trusted);
    /* cache newly created service */
    g_hash_table_insert (manager->priv->path_service_map, 
        (gpointer)msgport_dbus_service_get_object_path (dbus_service),
        (gpointer)dbus_service);

    /* append to list of services */
    service_list = g_list_append (service_list, dbus_service);
    if (was_empty) {
        g_hash_table_insert (manager->priv->owner_service_map, owner, service_list);
    }

    return dbus_service;
}

MsgPortDbusService *
msgport_manager_get_service (
    MsgPortManager      *manager,
    MsgPortDbusManager  *owner,
    const gchar         *remote_port_name,
    gboolean             is_trusted,
    GError             **error)
{
    GList *service_list = NULL;
    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), NULL);

    service_list = g_hash_table_lookup (manager->priv->owner_service_map, owner);
    while (service_list != NULL) {
        MsgPortDbusService *dbus_service = MSGPORT_DBUS_SERVICE (service_list->data);

        if ( !g_strcmp0 (remote_port_name, msgport_dbus_service_get_port_name (dbus_service)) && 
             is_trusted == msgport_dbus_service_get_is_trusted (dbus_service)) {
            return dbus_service ;
        }

        service_list = service_list->next;
    }

    /* FIXME: return ENOTFOUND */
    return NULL;
}

MsgPortDbusService *
msgport_manager_get_service_by_path (
    MsgPortManager *manager,
    const gchar    *service_object_path,
    GError        **error)
{
    MsgPortDbusService *dbus_service = NULL;
    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), NULL);

    dbus_service = g_hash_table_lookup (manager->priv->path_service_map, service_object_path);

    if (!dbus_service) {
        /* FIXME: return ENOTFOUND error */
    }

    return dbus_service;
}

static void
_unref_dbus_manager_cb (gpointer data, gpointer user_data)
{
    MsgPortDbusService *service = MSGPORT_DBUS_SERVICE (data);
    MsgPortManager *manager = MSGPORT_MANAGER (user_data);
    const gchar *object_path = NULL;
    
    g_assert (manager);
    g_assert (service);
    object_path = msgport_dbus_service_get_object_path (service);

    DBG ("Unregistering service %s:%s(%s)", 
        msgport_dbus_manager_get_app_id (msgport_dbus_service_get_owner (service)),
        msgport_dbus_service_get_port_name (service),
        object_path);
    /* remove the service from object_path:service map,
     * as its being unregisted */
    g_hash_table_remove (manager->priv->path_service_map, object_path);
}

/*
 * unregister all the services owned by a client
 */
gboolean
msgport_manager_unregister_services (
    MsgPortManager     *manager,
    MsgPortDbusManager *owner)
{
DBG("{");
    GList *service_list = NULL;
    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), FALSE);

    /* fetch sevice list owned by the client */
    service_list = g_hash_table_lookup (manager->priv->owner_service_map, owner);
    if (!service_list) {
        DBG("   No service found");
        return FALSE;
    }

    /* remove all the service from the list */
    g_list_foreach (service_list, _unref_dbus_manager_cb, manager);
    g_hash_table_remove (manager->priv->owner_service_map, owner);
DBG("}");
    return TRUE;
}

/*
 * unregister a signle service for given object path
 */
gboolean
msgport_manager_unregister_service (
    MsgPortManager *manager,
    const gchar *service_object_path)
{
    MsgPortDbusService *service = NULL;
    MsgPortDbusManager *owner = NULL;
    GList *service_list = NULL, *new_service_list = NULL;
    g_return_val_if_fail (manager && MSGPORT_IS_MANAGER (manager), FALSE);
DBG ("{");
    service = g_hash_table_lookup (manager->priv->path_service_map, service_object_path);

    if (!service) return FALSE;

    owner = msgport_dbus_service_get_owner (service);

    service_list = g_hash_table_lookup (manager->priv->owner_service_map, owner);

    /* remove service from services list owned by the 'owner'*/
    new_service_list = g_list_remove (service_list, service);
    if (new_service_list != service_list) {
        g_hash_table_steal (manager->priv->owner_service_map, owner);
        /* update the new list on this owner */
        g_hash_table_insert (manager->priv->owner_service_map, owner, new_service_list);
    }

    /* remove from the object_path:servcie table */
    g_hash_table_remove (manager->priv->path_service_map, service_object_path);
DBG("}");
    return TRUE;
}

