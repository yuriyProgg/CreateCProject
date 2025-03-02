#include "module.h"
#include "cJSON.h"
#include <stdio.h>

char new_module(char module_name[64])
{
  char cwd[256];
  getcwd(cwd, 256); // получить текущую рабочую директорию
  char json_path[256];
  sprintf(json_path, "%s/ccp.json", cwd);
  FILE *config_file = fopen(json_path, "r");

  if (!config_file)
    return 2;

  char buffer[512];
  fread(buffer, 1, sizeof(buffer), config_file);
  fclose(config_file);

  cJSON *json = cJSON_Parse(buffer);
  if (!json)
  {
    cJSON_Delete(json);
    return 2;
  }
  cJSON *language = cJSON_GetObjectItem(json, "language");
  if (cJSON_IsString(language) && language->valuestring)
  {
    char include_path[256], src_path[256];
    sprintf(include_path, "%s/include", cwd);
    sprintf(src_path, "%s/src", cwd);

    if (!directory_exists(include_path) && !directory_exists(src_path))
    {
      cJSON_Delete(json);
      printf("Нет в текущем каталоге папок src и include.\n");
      return 3;
    }

    if (!strncmp(language->valuestring, "cpp", 3))
    {
      char cpp_path[256];
      char hpp_path[256];
      sprintf(cpp_path, "%s/src/%s.cpp", cwd, module_name);
      sprintf(hpp_path, "%s/include/%s.hpp", cwd, module_name);
      FILE *cpp_file = fopen(cpp_path, "w");
      FILE *hpp_file = fopen(hpp_path, "w");
      if (!cpp_file || !hpp_file)
        return 2;

      fprintf(cpp_file, "#include \"%s.hpp\"\n", module_name);
      fprintf(hpp_file, "#pragma once\n");

      fclose(cpp_file);
      fclose(hpp_file);
    }
    else
    {
      char c_path[256];
      char h_path[256];
      sprintf(c_path, "%s/src/%s.c", cwd, module_name);
      sprintf(h_path, "%s/include/%s.h", cwd, module_name);
      FILE *c_file = fopen(c_path, "w");
      FILE *h_file = fopen(h_path, "w");
      if (!c_file || !h_file)
        return 2;

      fprintf(c_file, "#include \"%s.h\"\n", module_name);
      fprintf(h_file, "#pragma once\n");

      fclose(c_file);
      fclose(h_file);
    }
  }
  else
  {
    cJSON_Delete(json);
    return 1;
  }
  cJSON_Delete(json);

  return 0;
}