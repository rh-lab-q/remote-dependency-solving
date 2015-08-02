//test_template.c - simple test suite template

#include "test_template.h"

START_TEST(test_template)				//name of suite with "test_" prefix
{
  fail_if(1 == 2);						//your testing code
}
END_TEST

Suite* template_suite(void)				//name of suite with "_suite" postfix
{
  Suite* s = suite_create("template"); 	//name of suite
  TCase* tc = tcase_create("Main");
  tcase_add_test(tc, test_template);	//name of suite with "test_" prefix
  suite_add_tcase(s, tc);
  return s;
}

//don't forget to add name of your .c test file to tests/CMakeLists.txt (line 6)