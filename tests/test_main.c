#include <check.h>
#include "test_dummy.h"

int main()
{
  int number_failed;
  
  Suite* dum_suite = dummy_suite();
  SRunner* sr = srunner_create(dum_suite);
  
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  
  return (number_failed == 0) ? 0 : 1;
}