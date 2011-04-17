AC_INIT(possum, 0.0)
AM_INIT_AUTOMAKE(foreign)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AM_PATH_PYTHON
PYTHON_INCLUDES=$($PYTHON -c "import distutils.sysconfig; print distutils.sysconfig.get_python_inc()")
AC_SUBST(PYTHON_INCLUDES)

AC_CONFIG_HEADERS(possum/config.h)

AC_OUTPUT(Makefile
          possum/Makefile)
