
# Prelude.
AC_PREREQ([2.60])

AC_INIT([archlogger], [0.1], [maxim.paymushkin@adakta.ru])
CHECK_MAJOR_VERSION=0
CHECK_MINOR_VERSION=1
CHECK_MICRO_VERSION=1
CHECK_VERSION=$CHECK_MAJOR_VERSION.$CHECK_MINOR_VERSION.$CHECK_MICRO_VERSION

# unique source file --- primitive safety check 
AC_CONFIG_SRCDIR([src/archlogger.cpp])

# place where extra autoconf macros are kept
AC_CONFIG_MACRO_DIR([m4])

# define things like _GNU_SOURCE appropriately
AC_USE_SYSTEM_EXTENSIONS

# really severe build strictness
AM_INIT_AUTOMAKE([subdir-objects])

# From patch 2803433, request system extensions to generate 64-bit safe code
AC_USE_SYSTEM_EXTENSIONS

AC_SUBST(CHECK_MAJOR_VERSION)
AC_SUBST(CHECK_MINOR_VERSION)
AC_SUBST(CHECK_MICRO_VERSION)
AC_SUBST(CHECK_VERSION)

# Configure options.
# allow `./configure --enable-silent-rules' and `make V=0'
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([no])])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
# for non-POSIX archivers like the one on OS X
# use m4_ifdef to work on older automake (1.11)
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_LIBTOOL

AM_CONDITIONAL([GCC], [test x$GCC = xyes])
AM_CONDITIONAL([GXX], [test x$GXX = xyes])

# set language C++
AC_LANG_PUSH([C++])

# check standard c++ headers
AC_CHECK_HEADER([algorithm], [], [AC_MSG_ERROR([Missing header : algorithm])])
AC_CHECK_HEADER([fstream], [], [AC_MSG_ERROR([Missing header : fstream])])
AC_CHECK_HEADER([iomanip], [], [AC_MSG_ERROR([Missing header : iomanip])])
AC_CHECK_HEADER([iostream], [], [AC_MSG_ERROR([Missing header : iostream])])
AC_CHECK_HEADER([map], [], [AC_MSG_ERROR([Missing header : map])])
AC_CHECK_HEADER([sstream], [], [AC_MSG_ERROR([Missing header : sstream])])
AC_CHECK_HEADER([stdexcept], [], [AC_MSG_ERROR([Missing header : stdexcept])])
AC_CHECK_HEADER([string], [], [AC_MSG_ERROR([Missing header : string])])
AC_CHECK_HEADER([vector], [], [AC_MSG_ERROR([Missing header : vector])])

# check boost headers
AC_CHECK_HEADER([boost/algorithm/string.hpp], [], [AC_MSG_ERROR([Missing header : boost/algorithm/string.hpp])])
AC_CHECK_HEADER([boost/algorithm/string/predicate.hpp], [], [AC_MSG_ERROR([Missing header : boost/algorithm/string/predicate.hpp])])
AC_CHECK_HEADER([boost/algorithm/string/replace.hpp], [], [AC_MSG_ERROR([Missing header : boost/algorithm/string/replace.hpp])])
AC_CHECK_HEADER([boost/array.hpp], [], [AC_MSG_ERROR([Missing header : boost/array.hpp])])
AC_CHECK_HEADER([boost/asio.hpp], [], [AC_MSG_ERROR([Missing header : boost/asio.hpp])])
AC_CHECK_HEADER([boost/bind.hpp], [], [AC_MSG_ERROR([Missing header : boost/bind.hpp])])
AC_CHECK_HEADER([boost/filesystem.hpp], [], [AC_MSG_ERROR([Missing header : boost/filesystem.hpp])])
AC_CHECK_HEADER([boost/format.hpp], [], [AC_MSG_ERROR([Missing header : boost/format.hpp])])
AC_CHECK_HEADER([boost/function.hpp], [], [AC_MSG_ERROR([Missing header : boost/function.hpp])])
AC_CHECK_HEADER([boost/make_shared.hpp], [], [AC_MSG_ERROR([Missing header : boost/make_shared.hpp])])
AC_CHECK_HEADER([boost/optional.hpp], [], [AC_MSG_ERROR([Missing header : boost/optional.hpp])])
AC_CHECK_HEADER([boost/property_tree/ini_parser.hpp], [], [AC_MSG_ERROR([Missing header : boost/property_tree/ini_parser.hpp])])
AC_CHECK_HEADER([boost/property_tree/ptree.hpp], [], [AC_MSG_ERROR([Missing header : boost/property_tree/ptree.hpp])])
AC_CHECK_HEADER([boost/range/algorithm.hpp], [], [AC_MSG_ERROR([Missing header : boost/range/algorithm.hpp])])
AC_CHECK_HEADER([boost/regex.hpp], [], [AC_MSG_ERROR([Missing header : boost/regex.hpp])])
AC_CHECK_HEADER([boost/shared_ptr.hpp], [], [AC_MSG_ERROR([Missing header : boost/shared_ptr.hpp])])

# unset language C++
AC_LANG_POP([C++])

# Check if floor is in the math library, and if so add -lm to LIBS
AC_CHECK_LIB([m], [floor])

# Check if clock_gettime, timer_create, timer_settime, and timer_delete are available in lib rt, and if so,
# add -lrt to LIBS
AC_CHECK_LIB([rt], [clock_gettime, timer_create, timer_settime, timer_delete])

# Check for required libraries
AC_CHECK_LIB(pthread, pthread_create,,
	[AC_MSG_ERROR([required library pthread missing])])

# Check for required boost libraries
AC_CHECK_LIB(boost_program_options, main,,
	[AC_MSG_ERROR([required library boost_program_option missing])])
AC_CHECK_LIB(boost_regex, main,,
	[AC_MSG_ERROR([required library boost_regex missing])])
AC_CHECK_LIB(boost_system, main,,
	[AC_MSG_ERROR([required library boost_system missing])])
AC_CHECK_LIB(boost_filesystem, main,,
	[AC_MSG_ERROR([required library boost_filesystem missing])])

# check that struct timespec is defined in time.h. If not, we need to
# define it in libcompat.h. Note the optional inclusion of pthread.h.
# On MinGW and MinGW-w64, the pthread.h file contains the timespec
# definition.
AC_CHECK_MEMBERS([struct timespec.tv_sec, struct timespec.tv_nsec], [], [AC_DEFINE_UNQUOTED(STRUCT_TIMESPEC_DEFINITION_MISSING, 1, "Need to define the timespec structure")], [
#include <time.h>
#if defined(HAVE_PTHREAD)
#include <pthread.h>
#endif /* HAVE_PTHREAD */
])

# check that struct itimerspec is defined in time.h. If not, we need to
# define it in libcompat.h.  Note the optional inclusion of pthread.h.
# On MinGW and MinGW-w64, the pthread.h file contains the itimerspec
# definition.
AC_CHECK_MEMBERS([struct itimerspec.it_interval, struct itimerspec.it_value], [], [AC_DEFINE_UNQUOTED(STRUCT_ITIMERSPEC_DEFINITION_MISSING, 1, "Need to define the itimerspec structure")], [
#include <time.h>
#if defined(HAVE_PTHREAD)
#include <pthread.h>
#endif /* HAVE_PTHREAD */
])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h stddef.h stdlib.h string.h sys/time.h unistd.h])

AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(long, 4)

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_REPLACE_FUNCS([alarm clock_gettime timer_create timer_settime timer_delete fileno localtime_r pipe putenv setenv sleep strdup strsignal unsetenv])
AC_CHECK_DECLS([alarm, clock_gettime, timer_create, timer_settime, timer_delete, fileno, localtime_r, pipe, putenv, setenv, sleep, strdup, strsignal, unsetenv])

AC_CHECK_FUNCS([setitimer])

# Checks for functions not available in Windows
if test "xtrue" = x"$enable_fork"; then
	AC_CHECK_FUNCS([fork], HAVE_FORK=1, HAVE_FORK=0)
else
	HAVE_FORK=0
fi
AC_SUBST(HAVE_FORK)
AC_CHECK_FUNCS([sigaction])

# Check if the system's snprintf (and its variations) are C99 compliant.
# If they are not, use the version in libcompat.
HW_FUNC_VSNPRINTF
HW_FUNC_SNPRINTF
HW_FUNC_VASPRINTF
HW_FUNC_ASPRINTF

# Output files
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
	Makefile
	src/Makefile])

AC_OUTPUT
