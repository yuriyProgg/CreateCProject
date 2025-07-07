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

void get_lang_ext(const char *buffer, char **path)
{
  cJSON *json = cJSON_Parse(buffer);
  if (!json)
  {
    fprintf(stderr, "Ошибка: Не удалось распарсить JSON\n");
    return;
  }

  cJSON *lang_ext = cJSON_GetObjectItem(json, "lang_ext");
  if (cJSON_IsString(lang_ext) && lang_ext->valuestring != NULL)
    *path = strdup(lang_ext->valuestring); // Копируем строку
  else
    fprintf(stderr, "Ошибка: 'lang_ext' не найден или не является строкой\n");

  cJSON_Delete(json);
}

void get_header_ext(const char *buffer, char **path)
{
  cJSON *json = cJSON_Parse(buffer);
  if (!json)
  {
    fprintf(stderr, "Ошибка: Не удалось распарсить JSON\n");
    return;
  }

  cJSON *header_ext = cJSON_GetObjectItem(json, "header_ext");
  if (cJSON_IsString(header_ext) && header_ext->valuestring != NULL)
    *path = strdup(header_ext->valuestring); // Копируем строку
  else
    fprintf(stderr, "Ошибка: 'header_ext' не найден или не является строкой\n");

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

  char json_path[PATH_SIZE * 2];
  snprintf(json_path, sizeof(json_path), "%s/ccp.json", cwd);

  FILE *config_file = fopen(json_path, "r");
  if (!config_file)
  {
    perror("Ошибка: Не удалось открыть файл конфигурации");
    return 2;
  }

  char buffer[PATH_SIZE];
  size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, config_file);
  fclose(config_file);
  buffer[bytes_read] = '\0'; // Завершаем строку

  cJSON *json = cJSON_Parse(buffer);
  if (!json)
  {
    fprintf(stderr, "Ошибка: Не удалось распарсить JSON\n");
    return 2;
  }

  char *src_dir = NULL, *include_dir = NULL, *lang_ext = NULL, *header_ext = NULL;
  char cfile_path[PATH_SIZE], hfile_path[PATH_SIZE];

  get_src_dir(buffer, &src_dir);
  get_include_dir(buffer, &include_dir);
  get_lang_ext(buffer, &lang_ext);
  get_header_ext(buffer, &header_ext);

  if (!src_dir || !include_dir || !lang_ext || !header_ext)
    return 1;

  if ((size_t)snprintf(cfile_path, sizeof(cfile_path), "%s/%s/%s.%s", cwd, src_dir, module_name, lang_ext) >= sizeof(cfile_path))
  {
    fprintf(stderr, "Слишком длинный путь\n");
    return 1;
  }

  FILE *cfile = fopen(cfile_path, "w");
  if (!cfile)
  {
    perror("Ошибка: Не удалось создать файл");
    return 2;
  }
  fprintf(cfile, "#include \"%s.%s\"\n", module_name, header_ext);
  fclose(cfile);

  if ((size_t)snprintf(hfile_path, sizeof(hfile_path), "%s/%s/%s.%s", cwd, include_dir, module_name, header_ext) >= sizeof(hfile_path))
  {
    fprintf(stderr, "Слишком длинный путь\n");
    return 1;
  }

  FILE *hfile = fopen(hfile_path, "w");
  if (!hfile)
  {
    perror("Ошибка: Не удалось создать файл");
    return 2;
  }
  fprintf(hfile, "#pragma once\n");
  fclose(hfile);

  cJSON_Delete(json);
  return 0;
}