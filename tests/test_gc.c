//test_params.h - suite for testing parameters parsing

#include "test_gc.h"

START_TEST(test_gc)
{
  /*CLIENT FUNCTIONS TESTING*/
  /*test 1*/mem_test();
  /*test 2*/socket_test();
}
END_TEST

Suite* gc_suite(void)
{
  Suite* s = suite_create("gc");
  TCase* tc = tcase_create("Main");
  tcase_add_test(tc, test_gc);
  suite_add_tcase(s, tc);
  return s;
}

void mem_test()
{
  Ssds_gc * gc = ssds_gc_get_header();
  fail_if(gc != NULL);

  ssds_gc_init();

  gc = ssds_gc_get_header();
  fail_if(gc == NULL);

  void * a = ssds_malloc(500);
  void * b = ssds_realloc(a,800);

  ssds_gc_cleanup();
  gc = ssds_gc_get_header();
  fail_if(gc != NULL);
}

void socket_test()
{
  Ssds_gc * gc = ssds_gc_get_header();
  fail_if(gc != NULL);

  ssds_gc_init();
  int socket, sock_bind, sock_accept;

  socket = ssds_socket(AF_INET, SOCK_STREAM, 0);
  fail_if(socket < 0);

  gc = ssds_gc_get_header();
  fail_if(gc == NULL);

  ssds_gc_cleanup();

  gc = ssds_gc_get_header();
  fail_if(gc != NULL);
}