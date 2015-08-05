//test_params.h - suite for testing parameters parsing

#include "test_params.h"

START_TEST(test_params)
{
  ssds_gc_init();

  /*CLIENT FUNCTIONS TESTING*/
  /*test 1*/no_parameters();
  /*test 2*/install();
  /*test 3*/both_install_and_chkdep();
           //failing: says no command provided [line 85],
           // while it should be both commands [line 88]
  /*test 4*/no_command_provided();
  
  ssds_gc_cleanup();
}
END_TEST

Suite* params_suite(void)
{
  Suite* s = suite_create("params");
  TCase* tc = tcase_create("Main");
  tcase_add_test(tc, test_params);
  suite_add_tcase(s, tc);
  return s;
}

void no_parameters()
{
  ParamOptsCl* params = init_params_cl();
  char *argv_tst[1] = {"./ssds-client"};

  int ret = parse_params_cl(1, argv_tst, params);
  fail_if(ret != 2);

  free_params_cl(params);
  return;
}

void install()
{
  ParamOptsCl* params = init_params_cl();
  char *argv_tst[3] = {"./ssds-client", "--install", "emacs"};

  int ret = parse_params_cl(3, argv_tst, params);
  //printf("ret is %d, command is %d\n", ret, params->command);
  fail_if(params->command != -1);
  fail_if(ret != 0);

  //free_params_cl(params);
  return;
}

void both_install_and_chkdep()
{
  ParamOptsCl* params = init_params_cl();
  char *argv_tst[5] = {"./ssds-client", "--install", "emacs", "--chkdep", "qwe"};

  int ret = parse_params_cl(5, argv_tst, params);
  //printf("ret is %d, command is %d, pkg_count is %d\n", ret, params->command, params->pkg_count);
  fail_if(ret != -1);

  free_params_cl(params);
  return;
}

void no_command_provided()
{
  ParamOptsCl* params = init_params_cl();
  char *argv_tst[3] = {"./ssds-client", "emacs", "qwe"};

  int ret = parse_params_cl(3, argv_tst, params);
  //printf("ret is %d, command is %d, pkg_count is %d\n", ret, params->command, params->pkg_count);
  fail_if(ret != 3);

  free_params_cl(params);
  return;
}

