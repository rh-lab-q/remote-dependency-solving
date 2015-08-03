//test_template.c - simple test suite template

#include "test_cfg_parsing.h"

START_TEST(test_cfg_parsing)
{
  fail_if(1 == 2);

  FILE *tst_file_1;
  tst_file_1 = fopen("../connection.cfg", "w+");
  if (tst_file_1 == NULL)
  {
  	printf("unable to test, cannot create new file\n");
  	return;
  }
  char *tst_str = "address=127.0.0.2\nport=951\ndata-port=789\n";
  //char *tst_str1 = "951\0";
  fwrite(tst_str, sizeof(char), 9, tst_file_1);
  fseek(tst_file_1, 0, SEEK_SET);
  printf("hello\n");
  char *address, p1[5], p2[5];
  read_cfg(&address, &p1, &p2);

  printf("hello\n");

  printf("adress:%s p1:%c%c%c\n",address, p1[0], p1[1], p1[2]);

  free(address);
  //printf("read:[%s]\n", tst_str2);
  //fail_if(951 != strtol(tst_str2, NULL, 10));


  fclose(tst_file_1);
  remove("../connection.cfg");
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