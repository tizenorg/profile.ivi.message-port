lib_LTLIBRARIES = libmessageport-dbus-glue.la 
NULL = 

dbus-manager-glue.c dbus-maanager-glue.h : org.tizen.messageport.Manager.xml
	$(AM_V_GEM)gdbus-codegen                                \
        --interface-prefix org.tizen.messageport \
        --c-namespace      MsgPort_Dbus_Glue     \
        --generate-c-code  dbus-manager-glue     \
        $<

dbus-service-glue.c dbus-service-glue.h : org.tizen.messageport.Service.xml
	$(AM_V_GEM)gdbus-codegen                                \
        --interface-prefix org.tizen.messageport \
        --c-namespace      MsgPort_Dbus_Glue     \
        --generate-c-code  dbus-service-glue     \
        $<

nodist_libmessageport_dbus_glue_la_SOURCES = \
    dbus-manager-glue.h \
    dbus-manager-glue.c \
    dubs-service-glue.h \
    dbus-service-glue.c \
    dbus-error.h \
    dbus-error.c \
    $(NULL)
    
libmessageport_dbus_glue_la_CPPFLAGS = \
    -I$(top_builddir) \
    $(GLIB_CFLAGS) $(GIO_CFLAGS) $(GIOUNIX_CFLAGS)

libmessageport_dbus_glue_la_LIBADD = \
    $(GLIB_LIBS) $(GIO_LIBS) $(GIOUNIX_LIBS)

CLEANFILES = 
