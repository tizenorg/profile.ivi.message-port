#ifndef __MSGPORT_MANAGER_H
#define __MSGPORT_MANAGER_H

typedef _MsgPortManager MsgPortManager;
typedef _MsgPortService MsgPortService;

MsgPortManager *
msgport_get_manager ();

int
msgport_manager_register_port (MsgPortManager *manager, const gchar *port_name, gboolean is_trusted);

int
msgport_manager_unregister_port (MsgPortManager *manager, int port_id);

int
msgport_manager_get_port_id (MsgPortManager *manager, const gchar *remote_app_id, const gchar *port_name, gboolean is_trusted);



#define /* __MSGPORT_MANAGER_PROXY_H */
