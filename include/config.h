#pragma once

typedef struct
{
  char *lang_ext;
  char *header_ext;
  char *src;
  char *include;
} JsonConfig;

char directory_exists(const char *path);
char create_json_config(char *folder_path, const char *lang_ext, const char *header_ext);
