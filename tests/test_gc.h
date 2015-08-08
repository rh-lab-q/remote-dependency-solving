
#ifndef _TEST_GC_H
#define _TEST_GC_H

#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/common/mem_management.h"

Suite* gc_suite(void);

void mem_test();
void socket_test();

#endif