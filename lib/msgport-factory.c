#include "msgport-factory.h"
#include "msgport-manager.h"
#include "common/log.h"
#include <glib.h>

GHashTable *__managers = NULL; /* GThread:MsgPortManager */
G_LOCK_DEFINE(managers);

static 
void msgport_factory_init ()
{
    G_LOCK(managers);

    if (!__managers)
        __managers = g_hash_table_new_full (g_direct_hash, g_direct_equal,
                              NULL, (GDestroyNotify)g_object_unref);

    G_UNLOCK(managers);
}

void msgport_factory_uninit ()
{
    G_LOCK(managers);

    if (__managers) {
        g_hash_table_destroy (__managers);
        __managers = NULL;
    }

    G_UNLOCK(managers);
}

MsgPortManager * msgport_factory_get_manager () 
{
    MsgPortManager *manager = NULL;
    GThread *self_thread = g_thread_self ();

    if (!__managers) msgport_factory_init ();

    G_LOCK(managers);

    manager = MSGPORT_MANAGER (g_hash_table_lookup (__managers, self_thread));

    if (!manager) {
        manager = msgport_manager_new ();

        if (manager) 
            g_hash_table_insert (__managers, (gpointer)self_thread, manager);
    }

    G_UNLOCK(managers);

    return manager;
}
