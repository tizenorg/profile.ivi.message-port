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

#include "service.h"
#include "common/log.h"

G_DEFINE_TYPE (MsgPortService, msgport_service, G_TYPE_OBJECT)

#define MSGPORT_SERVICE_GET_PRIV(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE ((obj), MSGPORT_TYPE_SERVICE, MsgPortServicePrivate)

struct _MsgPortServicePrivate {
    gchar           *app_id;
    gchar           *port_name;
    gboolean         is_trusted;
};

static void
_service_finalize (GObject *self)
{
    MsgPortService *service = MSGPORT_SERVICE (self);

    g_free (service->priv->app_id);
    g_free (service->priv->port_name);

    G_OBJECT_CLASS (msgport_service_parent_class)->finalize (self);
}

static void
_service_dispose (GObject *self)
{
    //MsgPortService *service = MSGPORT_SERVICE (self);

    G_OBJECT_CLASS (msgport_service_parent_class)->dispose (self);
}

static void
msgport_service_init (MsgPortService *self)
{
    MsgPortServicePrivate *priv = MSGPORT_SERVICE_GET_PRIV (self);

    priv->app_id = NULL;
    priv->port_name = NULL;
    priv->is_trusted = FALSE;

    self->priv = priv;
}

static void
msgport_service_class_init (MsgPortServiceClass *klass)
{
    GObjectClass *gklass = G_OBJECT_CLASS(klass);

    g_type_class_add_private (klass, sizeof(MsgPortServicePrivate));

    gklass->finalize = _service_finalize;
    gklass->dispose = _service_dispose;
}

MsgPortService *
msgport_service_new (const gchar *app_id,
                      const gchar *port,
                      gboolean     is_trusted)
{
    MsgPortService *service = NULL;

    service= MSGPORT_SERVICE (g_object_new (MSGPORT_TYPE_SERVICE, NULL));
    if (!service) return NULL;

    service->priv->app_id = g_strdup (app_id);
    service->priv->port_name = g_strdup (port);
    service->priv->is_trusted = is_trusted;

    return service;
}

const gchar *
msgport_service_get_app_id (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), NULL);
    
    return (const gchar *)service->priv->app_id;
}

const gchar *
msgport_service_get_port_name (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), NULL);

    return (const gchar *)service->priv->port_name;
}

gboolean
msgport_service_get_is_trusted (MsgPortService *service)
{
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), FALSE);

    return service->priv->is_trusted;
}

GVariant *
msgport_service_to_variant (MsgPortService *service)
{
    GVariantBuilder v_builder;
    g_return_val_if_fail (service && MSGPORT_IS_SERVICE (service), NULL);

    g_variant_builder_init (&v_builder, G_VARIANT_TYPE_VARDICT);

    g_variant_builder_add (&v_builder, "{sv}", "AappId",
            g_variant_new_string (service->priv->app_id));
    g_variant_builder_add (&v_builder, "{sv}", "PortName",
            g_variant_new_string (service->priv->port_name));
    g_variant_builder_add (&v_builder, "{sv}", "IsTrusted",
            g_variant_new_boolean (service->priv->is_trusted));

    return g_variant_builder_end (&v_builder);
}

