//test_template.h - simple test suite template

#ifndef _TEST_CFG_PARSING_H
#define _TEST_CFG_PARSING_H

#include <check.h>
#include <stdio.h>
#include <stdlib.h>
//#include <cstdlib>

#include "../src/common/cfg_parsing.h"

Suite* cfg_parsing_suite(void);

int number_read();
int invalid_file();
int param_too_long();
int complex_read();

#endif
