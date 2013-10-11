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

#include "dbus-service.h"
#include "common/dbus-service-glue.h"
#include "common/log.h"
#include "manager.h"

G_DEFINE_TYPE (MsgPortDbusService, msgport_dbus_service, G_TYPE_OBJECT)

#define MSGPORT_DBUS_SERVICE_GET_PRIV(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE ((obj), MSGPORT_TYPE_DBUS_SERVICE, MsgPortDbusServicePrivate)

struct _MsgPortDbusServicePrivate {
    MsgPortDbusManager *owner;
    MsgPortDbusGlueService *dbus_skeleton;
    MsgPortService *service;
    gchar *object_path;
    guint id;
};


static void
_dbus_service_finalize (GObject *self)
{
    MsgPortDbusService *dbus_service = MSGPORT_DBUS_SERVICE (self);

    if (dbus_service->priv->object_path) {
        g_free (dbus_service->priv->object_path);
        dbus_service->priv->object_path = NULL;
    }

    G_OBJECT_CLASS (msgport_dbus_service_parent_class)->finalize (self);
}

static void
_dbus_service_dispose (GObject *self)
{
    MsgPortDbusService *dbus_service = MSGPORT_DBUS_SERVICE (self);
    DBG ("Unregistering service '%s'", 
        msgport_service_get_port_name (dbus_service->priv->service));
    if (dbus_service->priv->dbus_skeleton) {
        g_dbus_interface_skeleton_unexport (
                G_DBUS_INTERFACE_SKELETON (dbus_service->priv->dbus_skeleton));
        g_clear_object (&dbus_service->priv->dbus_skeleton);
    }

    g_clear_object (&dbus_service->priv->service);

//    g_clear_object (&dbus_service->priv->owner);

    G_OBJECT_CLASS (msgport_dbus_service_parent_class)->dispose (self);
}


static gboolean
_dbus_service_handle_send_message (
    MsgPortDbusService    *dbus_service,
    GDBusMethodInvocation *invocation,
    guint                   remote_service_id,
    GVariant              *data,
    gpointer               userdata)
{
    MsgPortDbusService *peer_dbus_service = NULL;
    MsgPortManager *manager = NULL;
    GError *error;
    g_return_val_if_fail (dbus_service &&  MSGPORT_IS_DBUS_SERVICE (dbus_service), FALSE);

    manager = msgport_dbus_manager_get_manager (dbus_service->priv->owner);
    peer_dbus_service = msgport_manager_get_service_by_id (manager, remote_service_id);
    if (!peer_dbus_service) {
        /* FIXME: return ENOTFOUND error */
        g_dbus_method_invocation_take_error (invocation, error);
    }
    else {
        msgport_dbus_service_send_message (peer_dbus_service, data,
                msgport_dbus_service_get_app_id (dbus_service),
                msgport_dbus_service_get_port_name (dbus_service),
                msgport_dbus_service_get_is_trusted (dbus_service));
        msgport_dbus_glue_service_complete_send_message (
                dbus_service->priv->dbus_skeleton, invocation);
    }

    return TRUE;
}

static gboolean
_dbus_service_handle_unregister (
    MsgPortDbusService    *dbus_service,
    GDBusMethodInvocation *invocation,
    gpointer               userdata)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), FALSE);

    /* FIXME unregister */
    return TRUE;
}

static gboolean
_dbus_service_handle_get_properties (
    MsgPortDbusService    *dbus_service,
    GDBusMethodInvocation *invocation,
    gpointer               userdata)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), FALSE);

    msgport_dbus_glue_service_complete_get_properties (
            dbus_service->priv->dbus_skeleton,
            invocation,
            msgport_service_to_variant (dbus_service->priv->service));

    return TRUE;
}

static void
msgport_dbus_service_init (MsgPortDbusService *self)
{
    MsgPortDbusServicePrivate *priv = MSGPORT_DBUS_SERVICE_GET_PRIV (self);

    priv->dbus_skeleton = msgport_dbus_glue_service_skeleton_new ();
    priv->service = NULL;
    priv->owner = NULL;

    g_signal_connect_swapped (priv->dbus_skeleton, "handle-send-message",
                G_CALLBACK (_dbus_service_handle_send_message), (gpointer)self);
    g_signal_connect_swapped (priv->dbus_skeleton, "handle-unregister",
                G_CALLBACK (_dbus_service_handle_unregister), (gpointer)self);
    g_signal_connect_swapped (priv->dbus_skeleton, "handle-get-properties",
                G_CALLBACK (_dbus_service_handle_get_properties), (gpointer)self);

    self->priv = priv;
}

static void
msgport_dbus_service_class_init (MsgPortDbusServiceClass *klass)
{
    GObjectClass *gklass = G_OBJECT_CLASS(klass);

    g_type_class_add_private (klass, sizeof(MsgPortDbusServicePrivate));

    gklass->finalize = _dbus_service_finalize;
    gklass->dispose = _dbus_service_dispose;
}

MsgPortDbusService *
msgport_dbus_service_new (MsgPortDbusManager *owner, const gchar *name, gboolean is_trusted)
{
    static guint object_conter = 0;
    MsgPortDbusService *dbus_service = NULL;
    GDBusConnection *connection = NULL;
    gchar *object_path = 0;

    GError *error = NULL;

    connection = msgport_dbus_manager_get_connection (owner),
    dbus_service = MSGPORT_DBUS_SERVICE (g_object_new (MSGPORT_TYPE_DBUS_SERVICE, NULL));
    if (!dbus_service) return NULL;

    /* FIXME: better way of path creation */
    object_path = g_strdup_printf ("/%u", ++object_conter); 
    if (!g_dbus_interface_skeleton_export (
            G_DBUS_INTERFACE_SKELETON (dbus_service->priv->dbus_skeleton),
            connection,
            object_path,
            &error)) {
        g_print ("Failed to export dbus object on connection %p : %s",
                    connection, error->message);
        g_error_free (error);
        g_object_unref (dbus_service);
        g_free (object_path);
        return NULL;
    }
    dbus_service->priv->id = object_conter;
    dbus_service->priv->owner = /*g_object_ref*/ (owner);
    dbus_service->priv->object_path = object_path;
    dbus_service->priv->service = msgport_service_new (
            msgport_dbus_manager_get_app_id (owner), name, is_trusted);

    g_assert (dbus_service->priv->service);
    g_assert (MSGPORT_IS_SERVICE (dbus_service->priv->service));

    return dbus_service;
}

guint
msgport_dbus_service_get_id (MsgPortDbusService *dbus_service)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), 0);

    return dbus_service->priv->id;
}

const gchar *
msgport_dbus_service_get_object_path (MsgPortDbusService *dbus_service)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), NULL);

    return (const gchar *)dbus_service->priv->object_path ;
}

GDBusConnection *
msgport_dbus_service_get_connection (MsgPortDbusService *dbus_service)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), NULL);

    return msgport_dbus_manager_get_connection (dbus_service->priv->owner);
}

MsgPortService *
msgport_dbus_service_get_service (MsgPortDbusService *dbus_service)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), NULL);

    return dbus_service->priv->service;
}

MsgPortDbusManager *
msgport_dbus_service_get_owner (MsgPortDbusService *dbus_service)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), NULL);

    return dbus_service->priv->owner;
}

const gchar *
msgport_dbus_service_get_port_name (MsgPortDbusService *dbus_service)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), NULL);

    return msgport_service_get_port_name (dbus_service->priv->service);
}

const gchar *
msgport_dbus_service_get_app_id (MsgPortDbusService *dbus_service)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), NULL);

    return msgport_dbus_manager_get_app_id (dbus_service->priv->owner);
}

gboolean
msgport_dbus_service_get_is_trusted (MsgPortDbusService *dbus_service)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), FALSE);

    return msgport_service_get_is_trusted (dbus_service->priv->service);
}

gboolean
msgport_dbus_service_send_message (MsgPortDbusService *dbus_service, GVariant *data, const gchar *r_app_id, const gchar *r_port, gboolean r_is_trusted)
{
    g_return_val_if_fail (dbus_service && MSGPORT_IS_DBUS_SERVICE (dbus_service), FALSE);

    msgport_dbus_glue_service_emit_on_message (dbus_service->priv->dbus_skeleton, data, r_app_id, r_port, r_is_trusted);
    
    return TRUE;
}

