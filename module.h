#pragma once

#include "cJSON.h"

char new_module(char module_name[64]);
void get_include_path(cJSON *json, char *path[256], char *cwd[256]);
void get_src_path(cJSON *json, char *path[256], char *cwd[256]);
