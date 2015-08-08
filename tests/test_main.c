/*
  test_main.c - runs all created (and added) suites
*/

#include <check.h>

/* INCLUDE TEST SUITES:
*/
#include "test_dummy.h"
#include "test_template.h"

int main()
{
  int number_failed;
  //create and add test suites to run
  SRunner* sr = srunner_create(dummy_suite());

  /* ADD TEST SUITES TO RUN:
  */
  srunner_add_suite(sr, template_suite());


  //runs all added tests
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  
  return (number_failed == 0) ? 0 : 1;
}