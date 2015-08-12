//test_missing_repos.h - test suite for testing missing repos at clients device

#ifndef _TEST_MISSING_REPOS_H	 //name of suite in caps with "_TEST_" prefix and "_H" postfix
#define _TEST_MISSING_REPOS_H	 //

#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/common/detect_missing_repos.h"

Suite* missing_repos_suite(void);

void clean_run();
void one_repo_missing();
void missing_file();

#endif