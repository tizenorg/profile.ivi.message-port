#include <glib.h>
#include <unistd.h>
#include <stdlib.h>
#include <message-port.h>
#include <bundle.h>
#include "common/log.h"

GMainLoop *__loop = NULL;

static void _dump_data (const char *key, const int type, const bundle_keyval_t *kv, void *user_data)
{
    gchar *val = NULL;
    size_t size;
    bundle_keyval_get_basic_val ((bundle_keyval_t*)kv, (void**)&val, &size);
    DBG ("       %s - %s", key, val);
}

void (_on_child_got_message)(int port_id, const char* remote_app_id, const char* remote_port, gboolean trusted_message, bundle* data)
{
    gchar *name = NULL;
    messageport_get_local_port_name (port_id, &name),
    DBG ("CHILD: GOT MESSAGE at prot '%s' FROM :'%s' - '%s", name,
        remote_app_id ? remote_app_id : "unknwon app", remote_port ? remote_port : "unknwon");
    g_free (name);
    g_assert (data);

    bundle_foreach (data, _dump_data, NULL);
    g_main_loop_quit (__loop);
}

void (_on_parent_got_message)(int port_id, const char* remote_app_id, const char* remote_port, gboolean trusted_message, bundle* data)
{
    gchar *name = NULL;
    gboolean found = FALSE;
    bundle *b = NULL;
    messageport_get_local_port_name (port_id, &name),
    DBG ("PARENT: GOT MESSAGE at prot %s FROM :'%s' - '%s", name,
        remote_app_id ? remote_app_id : "unknwon app", remote_port ? remote_port : "unknwon");
    g_free (name);

    g_assert (data);

    bundle_foreach (data, _dump_data, NULL);

    messageport_error_e res = trusted_message ? messageport_check_trusted_remote_port (remote_app_id, remote_port, &found)
                                              : messageport_check_remote_port (remote_app_id, remote_port, &found);
    if (!found) {
        WARN ("PARENT: Could not found remote port (%d)", res);
        exit (-1);
    }

    DBG ("PARENT: Found remote prot");

    bundle *reply = bundle_create ();

    bundle_add (reply, "Results", "GOT_IT");

    DBG ("PARENT: Sending reply ....");
    res = trusted_message ? messageport_send_trusted_message (remote_app_id, remote_port, reply)
                          : messageport_send_message (remote_app_id, remote_port, reply);
    bundle_free (reply);
    if (res != MESSAGEPORT_ERROR_NONE)
    {
        WARN ("PARENT: Faile to send message to server : %d", res);
    }
    else DBG ("PARENT: Data sent successfully");
    g_main_loop_quit (__loop);

}

int _register_test_port (const gchar *port_name, messageport_message_cb cb)
{
    int port_id = messageport_register_trusted_local_port (port_name, cb);

    if (port_id > MESSAGEPORT_ERROR_NONE) {
        gchar *name = NULL;
        messageport_get_local_port_name (port_id, &name); 
        g_free (name);
    }
    else {
        WARN ("Failed to register port : %d", port_id);
    }
    return port_id;
}

int main (int argc, char *argv[])
{
    pid_t child_pid;

    __loop = g_main_loop_new (NULL, FALSE);
    child_pid = fork ();
    
    if (child_pid < 0) {
        ERR ("Failed to fork ");
    }
    else if (child_pid > 0)  {
        /* prent process : server port */
       int port_id =  _register_test_port ("test_parent_port", _on_parent_got_message);
       if (port_id < 0) {
           WARN ("PARENT: Exiting...");
           exit (-1);
       }
       else {
           DBG ("PARENT ; registered port %d", port_id);
       }
    }
    else {
        /* child process */
        int port_id = _register_test_port ("test_child_port", _on_child_got_message);
        if (port_id < 0) {
           WARN ("CHILD: Exiting...");
           exit (-1);
        }
        else DBG ("CHILD ; registered port %d", port_id);

        DBG("CHILD: Waiting for sometime to get server port ready....");
        /* sleep sometime till server port is ready */
        sleep (3);

        gchar *parent_app_id = g_strdup_printf ("%d", getppid());
        gboolean found;
        messageport_error_e res = messageport_check_trusted_remote_port (parent_app_id, "test_parent_port", &found);

        if (!found) {
            WARN ("CHILD : Could not found remote port (%d)", res);
            exit(-1);
        }

        DBG ("CHILD : Found remote prot..., sending data to remote port (%s:%s)", parent_app_id, "test_parent_port");

        bundle *b = bundle_create ();
        bundle_add (b, "Name", "Amarnath");
        bundle_add (b, "Email", "amarnath.valluri@intel.com");

        res = messageport_send_bidirectional_trusted_message(port_id, parent_app_id, "test_parent_port", b);
        bundle_free (b);
        if (res != MESSAGEPORT_ERROR_NONE)
        {
            WARN ("CHILD: Fail to send message to server : %d", res);
            exit (-1);
        }
        else DBG ("CHILD : Data sent successfully");
    }

    g_main_loop_run (__loop);

    g_main_loop_unref (__loop);

    DBG ("TEST RSULT : SUCCESS");

    return 0;
}

