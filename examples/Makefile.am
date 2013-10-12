bin_PROGRAMS = msgport-example-app

msgport_example_app_SOURCES = test-app.c 

msgport_example_app_LDADD = ../lib/libmessage-port.la $(GLIB_LIBS) $(BUNDLE_LIBS)

msgport_example_app_CPPFLAGS  = -I../lib/ -I ../ $(GLIB_CFLAGS) $(BUNDLE_CFLAGS)

