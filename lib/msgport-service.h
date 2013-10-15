#ifndef __MSGPORT_SERVICE_H
#define __MSGPORT_SERVICE_H

#include <glib.h>
#include <gio/gio.h>
#include <glib-object.h>
#include <message-port.h>

G_BEGIN_DECLS

#define MSGPORT_TYPE_SERVICE          (msgport_service_get_type())
#define MSGPORT_SERVICE(obj)          (G_TYPE_CHECK_INSTANCE_CAST((obj), MSGPORT_TYPE_SERVICE, MsgPortService))
#define MSGPORT_IS_SERVICE(obj)       (G_TYPE_CHECK_INSTANCE_TYPE((obj), MSGPORT_TYPE_SERVICE))
#define MSGPORT_SERVICE_CLASS(cls)    (G_TYPE_CHECK_CLASS_CAST((cls), MSGPORT_TYPE_SERVICE, MsgPortServiceClass))
#define MSGPORT_IS_SERVICE_CLASS(cls) (G_TYPE_CHECK_CLASS_TYPE((cls), MSGPORT_TYPE_SERVICE))

typedef struct _MsgPortService MsgPortService;
typedef struct _MsgPortServiceClass MsgPortServiceClass;

struct _MsgPortServiceClass
{
    GObjectClass parent_class;
};

GType msgport_service_get_type(void);

MsgPortService *
msgport_service_new (GDBusConnection *connection, const gchar *path, messageport_message_cb message_cb);

const gchar *
msgport_service_name (MsgPortService *service);

gboolean
msgport_service_is_trusted (MsgPortService *service);

guint
msgport_service_id (MsgPortService *service);

gboolean
msgport_service_unregister (MsgPortService *service);

messageport_error_e
msgport_service_send_message (MsgPortService *service, guint remote_service_id, GVariant *message);

G_END_DECLS

#endif /* __MSGPORT_SERVICE_H */
