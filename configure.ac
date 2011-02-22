AC_INIT(tiny-couch, 0.1, rob.taylor@codethink.co.uk)
AC_PROG_CC
AM_INIT_AUTOMAKE([foreign])

AC_CHECK_LIB([crypto], [SHA1_Update], [LIBS="$LIBS -lcrypto"], [
	AC_MSG_ERROR([crypto library not found])
])

LT_INIT

PKG_CHECK_MODULES([GLIB], [glib-2.0])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(LIBSOUP, [libsoup-2.4 >= 2.26])
AC_SUBST(LIBSOUP_CFLAGS)
AC_SUBST(LIBSOUP_LIBS)

AC_CHECK_FUNCS([setproctitle setresgid auth_userokay getpeereid])
AC_REPLACE_FUNCS([memrchr strlcpy strlcat strtonum arc4random])
AC_CHECK_FUNC(strvisx, , [AC_LIBOBJ(vis)])
AC_CHECK_FUNC(arc4random_buf, , [AC_LIBOBJ(arc4random)])

AC_CONFIG_SUBDIRS([vendor/json-glib])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile compat/Makefile])
AC_OUTPUT

