//test_missing_repos.c - test suite for testing missing repos at clients device

#include "test_missing_repos.h"

START_TEST(test_missing_repos)
{
//   rds_gc_init();

  clean_run(); //if this test keeps failing, maybe you have problems with required repositaries
  one_repo_missing();
  missing_file();

//   rds_gc_cleanup();
}
END_TEST

Suite* missing_repos_suite(void)
{
  Suite* s = suite_create("missing_repos");
  TCase* tc = tcase_create("Main");
  tcase_add_test(tc, test_missing_repos);
  suite_add_tcase(s, tc);
  return s;
}

void clean_run()
{
	FILE *tempfile;
  tempfile = fopen("../required_repos_list.txt", "w+");
  if (tempfile == NULL)
  {
    printf("unable to test, cannot create new file\n");
    return ;
  }
  char *tst_str = "fedora.repo\n\
fedora-updates.repo\n\
fedora-updates-testing.repo\n\
google-chrome.repo\n\
ozonos.repo\n\
rpmfusion-free-rawhide.repo\n\
rpmfusion-free.repo\n\
rpmfusion-free-updates.repo\n\
rpmfusion-free-updates-testing.repo\n\
rpmfusion-nonfree-rawhide.repo\n\
rpmfusion-nonfree.repo\n\
rpmfusion-nonfree-updates.repo\n\
rpmfusion-nonfree-updates-testing.repo\n";

  fwrite(tst_str, sizeof(char), strlen(tst_str), tempfile);
  fseek(tempfile, 0, SEEK_SET);

  int t = check_for_missing_repos();
  fail_if(t != 0);

  fclose(tempfile);
  remove("../required_repos_list.txt");
}

void one_repo_missing()
{
	FILE *tempfile;
  tempfile = fopen("../required_repos_list.txt", "w+");
  if (tempfile == NULL)
  {
    printf("unable to test, cannot create new file\n");
    return;
  }
  char *tst_str = "repo_name_i_just_made_up\n";

  fwrite(tst_str, sizeof(char), strlen(tst_str), tempfile);
  fseek(tempfile, 0, SEEK_SET);

  int t = check_for_missing_repos();
  fail_if(t != 1);

  fclose(tempfile);
  remove("../required_repos_list.txt");
}

void missing_file()
{
	int t = check_for_missing_repos();
  fail_if(t != -1);
}