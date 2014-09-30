bin_PROGRAMS=archlogger test_server

archlogger_CFLAGS = $(GCC_AM_CFLAGS) -D_GNU_SOURCE
archlogger_CXXFLAGS = $(GXX_AM_CXXFLAGS) -D_GNU_SOURCE

archlogger_SOURCES = \
	csv/csv_worker.cpp \
	helper/log_helper.cpp \
	ini/log_ini.cpp \
	syslog/log_syslog.cpp \
	swa/swa_parser.cpp \
	logtype.cpp \
	logtype_timefolder.cpp \
	archlogger.cpp

test_server_CFLAGS = $(GCC_AM_CFLAGS) -D_GNU_SOURCE
test_server_CXXFLAGS = $(GXX_AM_CXXFLAGS) -D_GNU_SOURCE

test_server_SOURCES = \
	test/main.cpp

MAINTAINERCLEANFILES = aclocal.m4 compile config.guess config.sub configure depcomp install-sh ltmain.sh Makefile.in missing
