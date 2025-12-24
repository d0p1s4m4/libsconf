lib_LTLIBRARIES = libsconf.la
libsconf_la_SOURCES = sconf.c
include_HEADERS = sconf.h

EXTRA_DIST = LICENSE

man_MANS = sconf.3

pkgconfig_DATA = sconf.pc

LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/build-aux/tap-driver.sh

TESTS = $(check_PROGRAMS)

check_PROGRAMS = test_parse test_api
test_parse_SOURCES = tests/test_parse.c
test_parse_LDADD = -lcmocka libsconf.la
test_parse_CPPFLAGS = -I$(top_srcdir)

test_api_SOURCES = tests/test_api.c
test_api_LDADD = -lcmocka libsconf.la
test_api_CPPFLAGS = -I$(top_srcdir)
