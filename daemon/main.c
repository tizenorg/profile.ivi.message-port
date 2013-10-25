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

#include <glib.h>
#include "common/log.h"
#include "dbus-server.h"

static gboolean
_on_unix_signal (gpointer data)
{
    g_main_loop_quit ((GMainLoop*)data);

    return FALSE;
}

int main (int argc, char *argv[])
{
    GMainLoop *main_loop = NULL;
    MsgPortDbusServer *server = NULL;

#if !GLIB_CHECK_VERSION (2, 36, 0)
    g_type_init (&argc, &argv);
#endif

    main_loop = g_main_loop_new (NULL, FALSE);

    server = msgport_dbus_server_new();

    DBG ("server started at : %s", msgport_dbus_server_get_address (server));

    g_unix_signal_add (SIGTERM, _on_unix_signal, main_loop);
    g_unix_signal_add (SIGINT, _on_unix_signal, main_loop);

    g_main_loop_run (main_loop);

    g_object_unref (server);
    g_main_loop_unref (main_loop);

    DBG("Clean close");
}
