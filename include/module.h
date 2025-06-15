#pragma once

#include "cJSON.h"

char new_module(const char *module_name);
void get_include_dir(const char *buffer, char **path);
void get_src_dir(const char *buffer, char **path);
