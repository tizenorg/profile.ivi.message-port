#ifndef __MSGPORT_MANAGER_H
#define __MSGPORT_MANAGER_H

#include <glib.h>
#include <glib-object.h>
#include <message-port.h>

#define MSGPORT_TYPE_MANAGER (msgport_manager_get_type())
#define MSGPORT_MANAGER(obj)          (G_TYPE_CHECK_INSTANCE_CAST((obj), MSGPORT_TYPE_MANAGER, MsgPortManager))
#define MSGPORT_MANAGER_CLASS(kls)    (G_TYPE_CHECK_CLASS_CAST((kls), MSGPORT_TYPE_MANAGER, MsgPortManagerClass))
#define MSGPORT_IS_MANAGER(obj)       (G_TYPE_CHECK_INSTANCE_TYPE((obj), MSGPORT_TYPE_MANAGER))
#define MSGPORT_IS_MANAGER_CLASS(kls) (G_TYPE_CHECK_CLASS_TYPE((kls), MSGPORT_TYPE_MANAGER)

typedef struct _MsgPortManager MsgPortManager;
typedef struct _MsgPortManagerClass MsgPortManagerClass;
typedef struct _MsgPortService MsgPortService;

G_BEGIN_DECLS

struct _MsgPortManagerClass
{
    GObjectClass parent_class;
};

GType msgport_manager_get_type (void);

MsgPortManager *
msgport_get_manager ();

messageport_error_e
msgport_manager_register_service (MsgPortManager *manager, const gchar *port_name, gboolean is_trusted, messageport_message_cb cb, int *service_id_out);

messageport_error_e
msgport_manager_check_remote_service (MsgPortManager *manager, const gchar *remote_app_id, const gchar *port_name, gboolean is_trusted, guint *service_id_out);

messageport_error_e
msgport_manager_get_service_name (MsgPortManager *manager, int port_id, gchar **name_out);

messageport_error_e
msgport_manager_unregister_service (MsgPortManager *manager, int service_id);

messageport_error_e
msgport_manager_send_message (MsgPortManager *manager, const gchar *remote_app_id, const gchar *port_name, gboolean is_trusted, GVariant *data);

messageport_error_e
msgport_manager_send_bidirectional_message (MsgPortManager *manager, int from_id, const gchar *remote_app_id, const gchar *port_name, gboolean is_trusted, GVariant *data);

G_END_DECLS

#endif /* __MSGPORT_MANAGER_PROXY_H */
