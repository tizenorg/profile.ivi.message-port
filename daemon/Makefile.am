bin_PROGRAMS = messageportd
NULL = 

messageportd_SOURCES = \
    dbus-service.h \
    dbus-service.c \
    dbus-manager.h \
    dbus-manager.c \
    dbus-server.h \
    dbus-server.c \
    manager.h \
    manager.c \
    main.c \
    $(NULL)

messageportd_CPPFLAGS = \
    -I$(top_builddir) \
    $(GLIB_CLFAGS) $(GIO_CFLAGS) $(AUL_CFLAGS) $(PKGMGRINFO_CFLAGS)\
    $(NULL)

messageportd_LDADD = \
    ../common/libmessageport-common.la \
    $(GLIB_LIBS) $(GIO_LIBS) $(AUL_LIBS) $(PKGMGRINFO_LIBS) \
    $(NULL)

CLEANFILES = 
