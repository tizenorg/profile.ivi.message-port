#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT([message-port], [0.0.1], [], [], [])
AC_CONFIG_SRCDIR([daemon/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 nostdinc silent-rules subdir-objects tar-pax -Wno-portability])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([MESSAGEPORT],
                  [glib-2.0 >= 2.30
                   gio-2.0
                   gio-unix-2.0])
AC_SUBST(MESSAGEPORT_CFLAGS)
AC_SUBST(MESSAGEPORT_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT([
Makefile
daemon/Makefile])
