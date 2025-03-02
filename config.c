#include "config.h"
#include "cJSON.h"
#include <stdio.h>

#include <unistd.h>
#include <sys/stat.h>

char directory_exists(const char *path)
{
  struct stat info;

  if (stat(path, &info) != 0)
  {
    return 0; // Директория не существует или произошла ошибка
  }
  else if (S_ISDIR(info.st_mode))
  {
    return 1; // Это директория
  }
  else
  {
    return 0; // Это не директория
  }
}

char create_json_config(char *folder_path[256], char is_cpp, char *include_dir, char *src_dir)
{
  cJSON *root = cJSON_CreateObject();
  if (is_cpp)
    cJSON_AddStringToObject(root, "language", "cpp");
  else
    cJSON_AddStringToObject(root, "language", "c");
  cJSON_AddStringToObject(root, "include", include_dir);
  cJSON_AddStringToObject(root, "src", src_dir);

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
