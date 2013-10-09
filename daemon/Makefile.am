bin_PROGRAMS = messageportd
NULL = 

dbus-manager-glue.c dbus-maanager-glue.h : org.tizen.messageport.Manager.xml
	gdbus-codegen                                \
        --interface-prefix org.tizen.messageport \
        --c-namespace      MsgPort_Dbus_Glue     \
        --generate-c-code  dbus-manager-glue     \
        $<

dbus-service-glue.c dbus-service-glue.h : org.tizen.messageport.Service.xml
	gdbus-codegen                                \
        --interface-prefix org.tizen.messageport \
        --c-namespace      MsgPort_Dbus_Glue     \
        --generate-c-code  dbus-service-glue     \
        $<

messageportd_SOURCES = \
    dbus-manager-glue.h \
    dbus-manager-glue.c \
    dubs-service-glue.h \
    dbus-service-glue.c \
    dbus-service.h \
    dbus-service.c \
    dbus-manager.h \
    dbus-manager.c \
    dbus-server.h \
    dbus-server.c \
    service.h \
    service.c \
    manager.h \
    manager.c \
    main.c \
    $(NULL)

messageportd_CPPFLAGS = \
    -I$(top_builddir) \
    $(MESSAGEPORT_CFLAGS)

messageportd_LDADD = \
    $(MESSAGEPORT_LIBS)

CLEANFILES = 
