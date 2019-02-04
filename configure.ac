# Prelude
AC_PREREQ([2.59])
AC_INIT([tmalloc], [1.0], [frank.gu968@outlook.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.11.2])

# unique source file --- primitive safety check
AC_CONFIG_SRCDIR([src/pool_alloc.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR

# Include libcheck
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])
AM_PROG_CC_C_O

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
 demo/Makefile
 test/Makefile
])
AC_OUTPUT
