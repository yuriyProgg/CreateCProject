#include "config.h"
#include "cJSON.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

char directory_exists(const char *path)
{
#ifdef _WIN32
  // Windows implementation
  DWORD attributes = GetFileAttributesA(path);
  if (attributes == INVALID_FILE_ATTRIBUTES)
    return 0; // Path doesn't exist or error occurred
  if (attributes & FILE_ATTRIBUTE_DIRECTORY)
    return 1; // It's a directory
  else
    return 0; // It's not a directory

#else
  // Unix/Linux implementation
  struct stat info;

  if (stat(path, &info) != 0)
    return 0; // Directory doesn't exist or error occurred
  else if (S_ISDIR(info.st_mode))
    return 1; // It's a directory
  else
    return 0; // It's not a directory
#endif
}

char create_json_config(char *folder_path, const char *lang_ext, const char *header_ext)
{
  cJSON *root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "lang_ext", lang_ext);
  cJSON_AddStringToObject(root, "header_ext", header_ext);
  cJSON_AddStringToObject(root, "include", "include");
  cJSON_AddStringToObject(root, "src", "src");

  char json_path[256];
  sprintf(json_path, "%s/ccp.json", folder_path);
  FILE *file = fopen(json_path, "w");
  if (!file) // Проверка на ошибку создания файла
    return 2;

  char *json_str = cJSON_Print(root);
  fprintf(file, "%s", json_str);
  fclose(file);

  cJSON_Delete(root);
  cJSON_free(json_str);
  return 0;
}

JsonConfig read_json_config(char *folder_path)
{
  JsonConfig config;
  cJSON *root = cJSON_Parse(folder_path);
  config.lang_ext = cJSON_GetObjectItem(root, "lang_ext")->valuestring;
  config.header_ext = cJSON_GetObjectItem(root, "header_ext")->valuestring;
  config.src = cJSON_GetObjectItem(root, "src")->valuestring;
  config.include = cJSON_GetObjectItem(root, "include")->valuestring;
  return config;
}
