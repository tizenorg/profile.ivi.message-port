#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT([message-port], [0.0.1], [], [], [])
AC_CONFIG_SRCDIR([daemon/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 nostdinc silent-rules subdir-objects tar-pax -Wno-portability])
LT_INIT([disable-static])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.30])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES([GIO],[gio-2.0])
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)

PKG_CHECK_MODULES([GIOUNIX],[gio-unix-2.0])
AC_SUBST(GIOUNIX_CFLAGS)
AC_SUBST(GIOUINX_LIBS)

PKG_CHECK_MODULES([BUNDLE], [bundle])
AC_SUBST(BUNDLE_CFLAGS)
AC_SUBST(BUNDLE_LIBS)

AC_DEFINE(MESSAGEPORT_BUS_ADDRESS, 
         ["unix:path=%s/.message-port", g_get_user_runtime_dir()],
         [messageport daemon server socket address])

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT([
Makefile
common/Makefile
daemon/Makefile
lib/Makefile
examples/Makefile
])
