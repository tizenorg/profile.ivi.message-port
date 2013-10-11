#ifndef __MSGPORT_UTILS_H
#define __MSGPORT_UTILS_

#include <bundle.h>
#include <glib.h>

GVariant *bundle_to_variant_map (bundle *b);
bundle   *bundle_from_variant_map (GVariant *v);

#endif /* __MSGPORT_UTILS_H */
