ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libsconf.la
libsconf_la_SOURCES = sconf.c
include_HEADERS = sconf.h

EXTRA_DIST = LICENSE

man_MANS = sconf.3

LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/build-aux/tap-driver.sh

TESTS = $(check_PROGRAMS)

check_PROGRAMS = test_parse
test_parse_SOURCES = tests/test_parse.c
test_parse_LDADD = -lcmocka libsconf.la
test_parse_CPPFLAGS = -I$(top_srcdir)
