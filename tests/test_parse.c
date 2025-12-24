#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include "sconf.h"

static void
test_parse_empty_list(void **state)
{
	const char *str = "()";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_LIST);
	assert_null(s->value.as_child);

	sconf_destroy(s);
}

static void
test_parse_comment_nil(void **state)
{
	const char *str = "; just nil with comment\nnil ; :)";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_NIL);

	sconf_destroy(s);
}

static void
test_parse_bool(void **state)
{
	const char *boolkeywords[4] = {
		"true",
		"yes",
		"false",
		"no"
	};
	struct sconf *s;
	int i;

	for (i = 0; i < 4; i++)
	{
		s = sconf_parse(boolkeywords[i]);
		assert_non_null(s);
		assert_int_equal(s->type, SCONF_T_BOOL);
		assert_int_equal(s->value.as_int,
						 (i >= 2) ? SCONF_FALSE : SCONF_TRUE);
		sconf_destroy(s);
	}
}

static void
test_parse_symbol(void **state)
{
	const char *str = "marx";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_SYMBOL);
	assert_string_equal(s->value.as_string, str);

	sconf_destroy(s);
}

static void
test_parse_list(void **state)
{
	const char *str = "( ; a list with multiple elem\n" \
		"true false ; some bool\n" \
		"random_sym ; a random symbol\n" \
		"no)";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_LIST);
	assert_non_null(s->value.as_child);
	assert_int_equal(sconf_list_size(s), 4);

	sconf_destroy(s);
}

static void
test_parse_list_with_sublist(void **state)
{
	const char *str = "(sym () () ())";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_LIST);
	assert_non_null(s->value.as_child);
	assert_int_equal(sconf_list_size(s), 4);

	sconf_destroy(s);
}

static void
test_parse_empty_list_unexpected_eof(void **state)
{
	const char *str = "( ; eof";
	struct sconf *s;

	s = sconf_parse(str);
	assert_null(s);
}

static void
test_parse_list_unexpected_eof(void **state)
{
	const char *str = "(true yes";
	struct sconf *s;

	s = sconf_parse(str);
	assert_null(s);
}

static void
test_parse_int(void **state)
{
	const char *str = "-123456";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_INT);
	assert_int_equal(s->value.as_int, -123456);

	sconf_destroy(s);
}

static void
test_parse_double(void **state)
{
	const char *str = "3.14";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_DOUBLE);
	assert_double_equal(s->value.as_double, 3.14, 0.00004);

	sconf_destroy(s);
}

static void
test_parse_int_base16(void **state)
{
	const char *str = "0xFF";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_INT);
	assert_int_equal(s->value.as_int, 255);

	sconf_destroy(s);
}

static void
test_parse_string(void **state)
{
	const char *input = "\"Hello, world\\n\"";
	const char *expected = "Hello, world\n";
	struct sconf *s;

	s = sconf_parse(input);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_STRING);
	assert_string_equal(s->value.as_string, expected);

	sconf_destroy(s);
}

static void
test_parse_list_of_string(void **state)
{
	const char *str = "(\"lol\" \"lulz\" \"all your base belong to us\")";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_LIST);
	assert_int_equal(sconf_list_size(s), 3);

	sconf_destroy(s);
}

static void
test_parse_string_unexpected_eof(void **state)
{
	const char *str = "\"trans right are human right";
	struct sconf *s;

	s = sconf_parse(str);
	assert_null(s);
}

static void
test_parse_char(void **state)
{
	const char *str = "\\o";
	struct sconf *s;

	s = sconf_parse(str);
	assert_non_null(s);
	assert_int_equal(s->type, SCONF_T_CHAR);
	assert_int_equal(s->value.as_int, 'o');

	sconf_destroy(s);
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_parse_empty_list),
		cmocka_unit_test(test_parse_comment_nil),
		cmocka_unit_test(test_parse_bool),
		cmocka_unit_test(test_parse_symbol),
		cmocka_unit_test(test_parse_list),
		cmocka_unit_test(test_parse_list_with_sublist),
		cmocka_unit_test(test_parse_empty_list_unexpected_eof),
		cmocka_unit_test(test_parse_list_unexpected_eof),
		cmocka_unit_test(test_parse_int),
		cmocka_unit_test(test_parse_double),
		cmocka_unit_test(test_parse_int_base16),
		cmocka_unit_test(test_parse_string),
		cmocka_unit_test(test_parse_list_of_string),
		cmocka_unit_test(test_parse_string_unexpected_eof),
		cmocka_unit_test(test_parse_char),
	};

	cmocka_set_message_output(CM_OUTPUT_TAP);

	return (cmocka_run_group_tests(tests, NULL, NULL));
}
