NULL = 
lib_LTLIBRARIES = libmessage-port.la
libmessage_port_la_SOURCES = \
    message-port.c \
    msgport-utils.h \
    msgport-utils.c \
    msgport-service.h \
    msgport-service.c \
    msgport-manager.h \
    msgport-manager.c \
    $(NULL)

libmessage_port_la_includedir = $(includedir)/
libmessage_port_la_include_HEADERS = \
    message-port.h  \
    $(NULL)

libmessage_port_la_CPPFLAGS = \
    -I . \
    -I $(top_builddir) \
    -I $(top_builddir)/daemon \
    $(GLIB_CFLAGS) $(GIO_CFLAGS) $(BUNDLE_CFLAGS) \
    -Wall -error
    $(NULL)

libmessage_port_la_LIBADD = \
    ../common/libmessageport-common.la \
    $(GLIB_LIBS) $(GIO_LIBS) $(BUNDLE_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = message-port.pc

CLEANFILES =
