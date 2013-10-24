bin_PROGRAMS = msgport-example-app msgport-example-app-cpp

msgport_example_app_SOURCES = test-app.c 
msgport_example_app_LDADD = ../lib/libmessage-port.la $(GLIB_LIBS) $(BUNDLE_LIBS) $(DLOG_LIBS)
msgport_example_app_CPPFLAGS  = -I../lib/ -I ../ $(GLIB_CFLAGS) $(BUNDLE_CFLAGS) $(DLOG_CFLAGS)

msgport_example_app_cpp_SOURCES = test-app.cpp
msgport_example_app_cpp_LDADD = ../lib/libmessage-port.la $(GLIB_LIBS) $(BUNDLE_LIBS) $(DLOG_LIBS)
msgport_example_app_cpp_CXXFLAGS  = -I../lib/ -I ../ $(GLIB_CFLAGS) $(BUNDLE_CFLAGS) $(DLOG_CFLAGS)

