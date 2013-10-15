#ifndef __MSGPORT_UTILS_H
#define __MSGPORT_UTILS_H

#include <bundle.h>
#include <glib.h>
#include <message-port.h>

GVariant *bundle_to_variant_map (bundle *b);
bundle   *bundle_from_variant_map (GVariant *v);

messageport_error_e msgport_daemon_error_to_error (const GError *error);

#endif /* __MSGPORT_UTILS_H */
