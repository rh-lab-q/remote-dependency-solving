#include "test_dummy.h"

START_TEST(test_dummy)
{
  char* attempt = ret_null();
  ck_assert_str_eq(attempt, NULL);
//   printf("Test dummy\n");
}
END_TEST

Suite* dummy_suite(void)
{
  Suite* s = suite_create("dummy");
  TCase* tc = tcase_create("Main");
  tcase_add_test(tc, test_dummy);
  suite_add_tcase(s, tc);
  return s;
}

char* ret_null(void)
{
  return NULL;
}
