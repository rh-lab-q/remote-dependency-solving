//test_params.h - suite for testing parameters parsing

#include "test_params.h"

START_TEST(test_params)
{
  ssds_gc_init();

  no_parameters();
  //there is an exit() called, return should be sufficient [params.c line:28]

  install();
  //free_params_cl() throws SIGABORT

  both_install_and_chkdep();
  //free_params_cl() throws SIGABORT

  no_command_provided();
  //there is an exit() called, return could be sufficient [params.c line:86]
      //maybe return on [params.c line:96] could return 0, to differ
  //parse_params_cl() throws SEGFAULT
  //free_params_cl() might throw SIGABORT
  
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
  fail_if(ret != 1);

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

  free_params_cl(params);
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
  fail_if(ret != 1);

  free_params_cl(params);
  return;
}

