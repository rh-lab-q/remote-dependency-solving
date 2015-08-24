//test_template.c - simple test suite template

#include "test_cfg_parsing.h"

START_TEST(test_cfg_parsing)
{
  ssds_gc_init();

  number_read();
  invalid_file();
  param_too_long();
  complex_read();

  ssds_gc_cleanup();
}
END_TEST

Suite* cfg_parsing_suite(void)
{
  Suite* s = suite_create("cfg_parsing");
  TCase* tc = tcase_create("Main");
  tcase_add_test(tc, test_cfg_parsing);	
  suite_add_tcase(s, tc);
  return s;
}

int complex_read()
{
  FILE *tempfile;
  tempfile = fopen("../connection.cfg", "w+");
  if (tempfile == NULL)
  {
    printf("unable to test, cannot create new file\n");
    return 1;
  }
  char *tst_str = "address=127.0.0.2\nport=951\ndata-port=789\n";

  fwrite(tst_str, sizeof(char), strlen(tst_str), tempfile);
  fseek(tempfile, 0, SEEK_SET);

  char *address, *id;
  long int p1, p2;
  read_cfg(&id, &address, &p1, &p2);

  fail_if(strcmp(address, "127.0.0.2") != 0);
  fail_if(p1 != 951);
  fail_if(p2 != 789);
  fail_if(id != NULL);

  ssds_free(address);
  fclose(tempfile);
  remove("../connection.cfg");

  return 0;
}

int invalid_file()
{
  FILE *tempfile;
  tempfile = fopen("../connection.cfg", "w+");
  if (tempfile == NULL)
  {
    printf("unable to test, cannot create new file\n");
    return 1;
  }

  char *tst_str = "so-much=127.0.0.2\ninvalid=951\nvalues=789\n#here\n#address=void";

  fwrite(tst_str, sizeof(char), strlen(tst_str), tempfile);
  fseek(tempfile, 0, SEEK_SET);

  char *address, *id;
  long int p1, p2;
  read_cfg(&id, &address, &p1, &p2);

  fail_if(strcmp(address, "127.0.0.1") != 0);
  fail_if(p1 != 2345);
  fail_if(p2 != 2346);
  fail_if(id != NULL);

  ssds_free(address);
  fclose(tempfile);
  remove("../connection.cfg");

  return 0;
}

int param_too_long()
{
  FILE *tempfile;
  tempfile = fopen("../connection.cfg", "w+");
  if (tempfile == NULL)
  {
    printf("unable to test, cannot create new file\n");
    return 1;
  }

  char *tst_str = "port=4815162342";

  fwrite(tst_str, sizeof(char), strlen(tst_str), tempfile);
  fseek(tempfile, 0, SEEK_SET);

  char *address, *id;
  long int p1, p2;
  read_cfg(&id, &address, &p1, &p2);

  fail_if(p1 != 4815);
  
  ssds_free(address);
  fclose(tempfile);
  remove("../connection.cfg");

  return 0;
}

int number_read()
{
  FILE *tempfile;
  tempfile = fopen("../connection.cfg", "w+");
  if (tempfile == NULL)
  {
    printf("unable to test, cannot create new file\n");
    return 1;
  }

  char *tst_str = "4815162342\nxxx";

  fwrite(tst_str, sizeof(char), strlen(tst_str), tempfile);

  fseek(tempfile, 0, SEEK_SET);
  char *res = file_read_value(tempfile, 0);
  fail_if(strcmp(res, "4815162342") != 0);

  ssds_free(res);

  fseek(tempfile, 0, SEEK_SET);
  file_read_value(tempfile, 4);
  fail_if(strcmp(res, "481") != 0);
  
  ssds_free(res);
  fclose(tempfile);
  remove("../connection.cfg");

  return 0;
}

