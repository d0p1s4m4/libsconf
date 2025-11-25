#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include "sconf.h"

static void
test_bool(void **state)
{
	struct sconf *s;

	s = sconf_new_true();
	assert_true(sconf_is_true(s));

	sconf_destroy(s);

	s = sconf_new_false();
	assert_true(sconf_is_bool(s));
	assert_true(sconf_is_false(s));

	sconf_destroy(s);
}

static void
test_list(void **state)
{
	struct sconf *lst;
	struct sconf *itm1;
	struct sconf *itm2;
	struct sconf *itm3;

	lst = sconf_new_list();
	assert_true(sconf_is_list(lst));
	assert_int_equal(sconf_list_size(lst), 0);

	itm1 = sconf_new_nil();
	itm2 = sconf_new_list();
	itm3 = sconf_new_int(123);

	sconf_list_appends(lst, itm1, itm2, itm3, SCONF_END);

	assert_int_equal(sconf_list_size(lst), 3);
	assert_ptr_equal(sconf_list_first(lst), itm1);
	assert_ptr_equal(sconf_list_at(lst, 1), itm2);
	assert_ptr_equal(sconf_list_at(lst, 2), itm3);

	sconf_list_remove(lst, itm2);

	assert_int_equal(sconf_list_size(lst), 2);
	assert_ptr_equal(sconf_list_at(lst, 0), itm1);
	assert_ptr_equal(sconf_list_at(lst, 1), itm3);

	sconf_list_remove(lst, itm1);

	assert_int_equal(sconf_list_size(lst), 1);
	assert_ptr_equal(sconf_list_first(lst), itm3);

	sconf_list_remove(lst, itm3);
	assert_int_equal(sconf_list_size(lst), 0);

	sconf_destroy(itm1);
	sconf_destroy(itm2);
	sconf_destroy(itm3);
	sconf_destroy(lst);
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_bool),
		cmocka_unit_test(test_list),
	};

	cmocka_set_message_output(CM_OUTPUT_TAP);

	return (cmocka_run_group_tests(tests, NULL, NULL));
}