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
