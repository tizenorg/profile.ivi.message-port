#include <glib.h>
#include <unistd.h>
#include <message-port.h>
#include <bundle.h>

GMainLoop *__loop = NULL;

static void _dump_data (const char *key, const int type, const bundle_keyval_t *kv, void *user_data)
{
    gchar *val = NULL;
    size_t size;
    bundle_keyval_get_basic_val ((bundle_keyval_t*)kv, (void**)&val, &size);
    g_print ("       %s - %s\n", key, val);
}

void (_on_got_message)(int port_id, const char* remote_app_id, const char* remote_port, gboolean trusted_message, bundle* data)
{
    gchar *name = NULL;
    messageport_get_local_port_name (port_id, &name),
    g_print ("SERVER: GOT MESSAGE at prot %s FROM :'%s' - '%s\n", name,
        remote_app_id ? remote_app_id : "unknwon app", remote_port ? remote_port : "unknwon");
    g_free (name);

    g_assert (data);

    bundle_foreach (data, _dump_data, NULL);

    g_main_loop_quit (__loop);
}

int main (int argc, char *argv[])
{
    const gchar *port_name = "test_port";
    pid_t child_pid;

    __loop = g_main_loop_new (NULL, FALSE);
    child_pid = fork ();
    
    if (child_pid < 0) {
        g_error ("Failed to fork ");
    }
    else if (child_pid > 0)  {
        /* prent process : server port */
        int port_id = messageport_register_local_port (port_name, _on_got_message);

        if (port_id > MESSAGEPORT_ERROR_NONE) {
            gchar *name = NULL;
            messageport_get_local_port_name (port_id, &name); 
            g_print ("Registerd Port : %s(Id: %d)\n", name, port_id);
            g_free (name);
        }
        else {
            g_print ("Failed to register port : %d \n", port_id);
            return -1;
        }
    }
    else {
        /* child process */
        /* sleep sometime till server port is ready */
        sleep (5);
        gchar *app_id = g_strdup_printf ("%d", getppid());
        gboolean found;
        messageport_error_e res = messageport_check_remote_port (app_id, port_name, &found);

        if (!found) {
            g_print ("CHILD : Could not found remote port (%d)", res);
            return -1;
        }

        g_print ("CHILD : Found remote prot\n");

        bundle *b = bundle_create ();

        bundle_add (b, "Name", "Amarnath");
        bundle_add (b, "Email", "amarnath.valluri@intel.com");

        g_print ("CHILD : Sending data ....\n");
        res = messageport_send_message (app_id, port_name, b);
        bundle_free (b);
        if (res != MESSAGEPORT_ERROR_NONE)
        {
            g_print ("CHILD: Faile to send message to server : %d", res);
        }
        else g_print ("CHILD : Data sent successfully");

        exit (0);
    }

    g_main_loop_run (__loop);

    g_main_loop_unref (__loop);

    return 0;
}

