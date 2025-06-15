#include "module.h"
#include "config.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void get_include_dir(const char *buffer, char **path)
{
  cJSON *json = cJSON_Parse(buffer);
  if (!json)
  {
    fprintf(stderr, "Ошибка: Не удалось распарсить JSON\n");
    return;
  }

  cJSON *include_path = cJSON_GetObjectItem(json, "include");
  if (cJSON_IsString(include_path) && include_path->valuestring != NULL)
    *path = strdup(include_path->valuestring); // Копируем строку
  else
    fprintf(stderr, "Ошибка: 'include' не найден или не является строкой\n");

  cJSON_Delete(json);
}

void get_src_dir(const char *buffer, char **path)
{
  cJSON *json = cJSON_Parse(buffer);
  if (!json)
  {
    fprintf(stderr, "Ошибка: Не удалось распарсить JSON\n");
    return;
  }

  cJSON *src_path = cJSON_GetObjectItem(json, "src");
  if (cJSON_IsString(src_path) && src_path->valuestring != NULL)
    *path = strdup(src_path->valuestring); // Копируем строку
  else
    fprintf(stderr, "Ошибка: 'src' не найден или не является строкой\n");

  cJSON_Delete(json);
}

char new_module(const char *module_name)
{
  char cwd[256];
  if (!getcwd(cwd, sizeof(cwd)))
  {
    perror("Ошибка: Не удалось получить текущую рабочую директорию");
    return 2;
  }

  char json_path[512];
  snprintf(json_path, sizeof(json_path), "%s/ccp.json", cwd);

  FILE *config_file = fopen(json_path, "r");
  if (!config_file)
  {
    perror("Ошибка: Не удалось открыть файл конфигурации");
    return 2;
  }

  char buffer[512];
  size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, config_file);
  fclose(config_file);
  buffer[bytes_read] = '\0'; // Завершаем строку

  cJSON *json = cJSON_Parse(buffer);
  if (!json)
  {
    fprintf(stderr, "Ошибка: Не удалось распарсить JSON\n");
    return 2;
  }

  cJSON *language = cJSON_GetObjectItem(json, "language");
  if (cJSON_IsString(language) && language->valuestring != NULL)
  {
    char *include_dir = NULL;
    char include_path[257];
    get_include_dir(buffer, &include_dir);
    if (include_dir)
    {
      snprintf(include_path, sizeof(include_path), "%s/%s", cwd, include_dir);
      free(include_dir);
    }
    else
    {
      fprintf(stderr, "Ошибка: Директория 'include' не найдена\n");
      cJSON_Delete(json);
      return 3;
    }

    char *src_dir = NULL;
    char src_path[257];
    get_src_dir(buffer, &src_dir);
    if (src_dir)
    {
      snprintf(src_path, sizeof(src_path), "%s/%s", cwd, src_dir);
      free(src_dir);
    }
    else
    {
      fprintf(stderr, "Ошибка: Директория 'src' не найдена\n");
      cJSON_Delete(json);
      return 3;
    }

    if (!directory_exists(include_path) || !directory_exists(src_path))
    {
      fprintf(stderr, "Ошибка: Директории 'include' или 'src' не существуют\n");
      cJSON_Delete(json);
      return 3;
    }

    if (strcmp(language->valuestring, "cpp") == 0)
    {
      char cpp_path[512], hpp_path[512];
      snprintf(cpp_path, sizeof(cpp_path), "%s/%s.cpp", src_path, module_name);
      snprintf(hpp_path, sizeof(hpp_path), "%s/%s.hpp", include_path, module_name);

      FILE *cpp_file = fopen(cpp_path, "w");
      FILE *hpp_file = fopen(hpp_path, "w");
      if (!cpp_file || !hpp_file)
      {
        perror("Ошибка: Не удалось создать файлы");
        cJSON_Delete(json);
        return 2;
      }

      fprintf(cpp_file, "#include \"%s.hpp\"\n", module_name);
      fprintf(hpp_file, "#pragma once\n");

      fclose(cpp_file);
      fclose(hpp_file);
    }
    else
    {
      char c_path[512], h_path[512];
      snprintf(c_path, sizeof(c_path), "%s/%s.c", src_path, module_name);
      snprintf(h_path, sizeof(h_path), "%s/%s.h", include_path, module_name);

      FILE *c_file = fopen(c_path, "w");
      FILE *h_file = fopen(h_path, "w");
      if (!c_file || !h_file)
      {
        perror("Ошибка: Не удалось создать файлы");
        cJSON_Delete(json);
        return 2;
      }

      fprintf(c_file, "#include \"%s.h\"\n", module_name);
      fprintf(h_file, "#pragma once\n");

      fclose(c_file);
      fclose(h_file);
    }
  }
  else
  {
    fprintf(stderr, "Ошибка: 'language' не найден или не является строкой\n");
    cJSON_Delete(json);
    return 1;
  }

  cJSON_Delete(json);
  return 0;
}