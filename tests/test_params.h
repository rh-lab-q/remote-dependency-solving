//test_params.h - suite for testing parameters parsing

#ifndef _TEST_PARAMS_H
#define _TEST_PARAMS_H

#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/common/params.h"
#include "../src/common/mem_management.h"

Suite* params_suite(void);

void no_parameters();
void install();
void both_install_and_chkdep();
void no_command_provided();

#endif