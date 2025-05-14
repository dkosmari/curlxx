#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([curlxx], [0.0], [])
AC_CONFIG_SRCDIR([src/curl.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([foreign subdir-objects])

# Checks for programs.
AC_PROG_CXX
AX_APPEND_COMPILE_FLAGS([-std=c++23], [CXX])
AC_LANG([C++])

AC_PROG_RANLIB


AC_ARG_ENABLE([static],
              [AS_HELP_STRING([--enable-static], [use static libcurl.])])

AS_IF([test $enable_static = xyes],
      [PKG_CHECK_MODULES_STATIC([CURL], [libcurl])],
      [PKG_CHECK_MODULES([CURL], [libcurl])]
)


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
