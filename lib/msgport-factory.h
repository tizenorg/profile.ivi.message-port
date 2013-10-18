#ifndef __MSGPORT_FACTORY_H
#define __MSGPORT_FACTORY_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _MsgPortManager MsgPortManager;

MsgPortManager * msgport_factory_get_manager ();

G_END_DECLS

#endif /* __MSGPORT_FACTORY_H */
