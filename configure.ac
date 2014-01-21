#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(tds, 0.01, buf1024@gmail.com)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC(cc)
AC_PROG_MAKE_SET

if test "x$CC" = "xcc" ; then
    LDFLAGS="$LDFLAGS"
    #DEBUG
    CCFLAGS="-ggdb -Wall -fPIC -fpic"
    #RELEASE
    #CCFLAGS="-ggdb -Wall -fPIC -fpic -O2 -DNDEBUG"
    LIB_CCFLAGS="-shared -fPIC -fpic"
    LIB_LDFLAGS="-shared -fPIC -fpic"
    ARCHIVE="ar"
    AR_FLAGS="-r -o"
fi

AC_SUBST(CCFLAGS)
AC_SUBST(LIB_CCFLAGS)
AC_SUBST(LIB_LDFLAGS)
AC_SUBST(ARCHIVE)
AC_SUBST(AR_FLAGS)

if test "x$prefix" = "xNONE" ; then
     prefix=`pwd`
fi

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])
AC_CHECK_HEADERS([string.h])
# Checks for typedefs, structures, and compiler characteristics.
#AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
#AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
#AC_FUNC_MKTIME
#AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
#AC_FUNC_VPRINTF
#AC_CHECK_FUNCS([alarm dup2 gettimeofday inet_ntoa localtime_r memset select socket strcasecmp strerror strrchr strtol])
#AC_CHECK_FUNCS([strlwr])

AC_OUTPUT([Makefile
	tds/Makefile
	src/Makefile
	test/Makefile
])
