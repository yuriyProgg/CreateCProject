#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "cJSON.h"

#ifdef __MINGW32__
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

char create_cmake_project(char *project_name, char is_cpp, char *std);
char create_make_project(char *project_name, char is_cpp, char *std);
char create_json_config(char *folder_path[256], char is_cpp);
char new_module();
char directory_exists(const char *path);

int main(int argc, char *argv[])
{
  // Проверка на количество аргументов
  if (argc <= 1 || !strncmp(argv[1], "-h", 2) || !strncmp(argv[1], "--help", 6))
  {
    printf("Используете `%s <Имя проекта или Команда> [Флаги]`.\n", argv[0]);
    puts("\nКоманды:");
    puts("  mod\t\t\tСоздать новый модуль (если в проекте есть папки src и include).");
    puts("\nФлаги:");
    puts("  -cpp\t\t\tИспользовать C++. По умолчанию используется C.");
    puts("  -std=<ЗНАЧЕНИЕ>\tСтандарт С\\С++. По умолчанию 23.");
    puts("  -cmake\t\tИспользовать CMake. По умолчанию используется Makefile.");
    puts("  -h, --help\t\tПоказать этот текст.");
    return 1;
  }
  else if (!strncmp(argv[1], "mod", 3))
  {
    char result = new_module();
    if (result == 2)
      printf("Ошибка создания файла\n");
    else if (result != 0)
      printf("Ошибка №%d\n", result);
    return result;
  }

  // Иницилизация переменных
  char *project_name = argv[1], is_cpp = 0, *standart = "23", *tool = "make";

  // Ищем дполнительные флаги
  for (int i = 0; i < argc; ++i)
  {
    if (!strncmp(argv[i], "-cmake", 6))
      tool = "cmake";
    if (!strncmp(argv[i], "-std=", 5))
      standart = argv[i] + 5;
    if (!strncmp(argv[i], "-cpp", 4))
      is_cpp = 1;
  }

  // Создаем проект
  char result = strncmp(tool, "make", 4) ? create_make_project(project_name, is_cpp, standart) : create_cmake_project(project_name, is_cpp, standart);
  if (result == 2)
    printf("Ошибка создания файла\n");
  else if (result != 0)
    printf("Ошибка №%d\n", result);
  return result;
}

// Создает проект на CMake
char create_cmake_project(char *project_name, char is_cpp, char *std)
{
  char cwd[256];
  getcwd(cwd, 256); // получить текущую рабочую директорию

  // создать папку
  char folder_path[256];
  sprintf(folder_path, "%s/%s", cwd, project_name);
  mkdir(folder_path, 0777);
  sprintf(folder_path, "%s/%s/src", cwd, project_name);
  mkdir(folder_path, 0777);
  sprintf(folder_path, "%s/%s/include", cwd, project_name);
  mkdir(folder_path, 0777);
  // Возращаем папку в корень
  sprintf(folder_path, "%s/%s", cwd, project_name);

  // создать файл CMakeLists.txt
  char cmakefile_path[256];
  sprintf(cmakefile_path, "%s/CMakeLists.txt", folder_path);
  FILE *file = fopen(cmakefile_path, "w");

  if (!file) // Проверка на ошибку создания файлаV
    return 2;

  fprintf(file, "cmake_minimum_required(VERSION 3.28)\n");
  fprintf(file, "project(%s)\n", project_name);
  if (is_cpp)
  {

    fprintf(file, "set(CMAKE_CXX_STANDARD %s)\n", std);
    fprintf(file, "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n");
    fprintf(file, "add_executable(${PROJECT_NAME} src/main.cpp)\n");
  }
  else
  {
    fprintf(file, "set(CMAKE_C_STANDARD %s)\n", std);
    fprintf(file, "set(CMAKE_C_STANDARD_REQUIRED ON)\n");
    fprintf(file, "add_executable(${PROJECT_NAME} src/main.c)\n");
  }
  fprintf(file, "target_include_directories(${PROJECT_NAME} PRIVATE include)\n");
  fclose(file);

  // создать файл main.cpp
  if (is_cpp)
  {
    char cppfile_path[255];
    sprintf(cppfile_path, "%s/src/main.cpp", folder_path);
    file = fopen(cppfile_path, "w");
    if (!file) // Проверка на ошибку создания файла
      return 2;
    fprintf(file, "#include <iostream>\n\nint main()\n{\n  std::cout << \"Hello, World!\" << std::endl;\n  return 0;\n}\n");
  }
  else
  {
    char cfile_path[255];
    sprintf(cfile_path, "%s/src/main.c", folder_path);
    file = fopen(cfile_path, "w");
    if (!file) // Проверка на ошибку создания файла
      return 2;
    fprintf(file, "#include <stdio.h>\n\nint main()\n{\n  printf(\"Hello, World!\");\n  return 0;\n}\n");
  }
  fclose(file);
  // Возращаем папку в корень
  sprintf(folder_path, "%s/%s", cwd, project_name);

  create_json_config(folder_path, is_cpp);
  return 0;
}

// Создает проект на Makefile
char create_make_project(char *project_name, char is_cpp, char *std)
{
  char cwd[256];
  getcwd(cwd, 256); // получить текущую рабочую директорию

  // Cоздаем папку
  char folder_path[256];
  sprintf(folder_path, "%s/%s", cwd, project_name);
  mkdir(folder_path, 0777);

  // Создаем Makefile
  char makefile_path[256];
  sprintf(makefile_path, "%s/Makefile", folder_path);
  FILE *file = fopen(makefile_path, "w");

  if (!file) // Проверка на ошибку создания файла
    return 2;

  fprintf(file, "TARGET=%s\n", project_name);
  if (is_cpp)
  {
    fprintf(file, "CC=g++\n");
    fprintf(file, "FLAGS=-Wall -Wextra -Werror -g -std=c++%s\n", std);
    fprintf(file, "SOURCES=$(wildcard *.cpp)\n");
    fprintf(file, "OBJECTS=$(SOURCES:.cpp=.o)\n");
  }
  else
  {
    fprintf(file, "CC=gcc\n");
    fprintf(file, "FLAGS=-Wall -Wextra -Werror -g\n");
    fprintf(file, "SOURCES=$(wildcard *.c)\n");
    fprintf(file, "OBJECTS=$(SOURCES:.c=.o)\n");
  }

  fprintf(file, "\nall: $(TARGET)\n");

  fprintf(file, "\n$(TARGET): $(OBJECTS)\n");
  fprintf(file, "\t$(CC) $(FLAGS) -o $@ $^\n");

  if (is_cpp)
    fprintf(file, "\n%%.o: %%.cpp\n");
  else
    fprintf(file, "\n%%.o: %%.c\n");
  fprintf(file, "\t$(CC) $(FLAGS) -c $<\n");

  fprintf(file, "\nclean:\n");
  fprintf(file, "\trm -f $(OBJECTS) $(TARGET)\n");

  fclose(file);

  // Создание С\С++ файла
  if (is_cpp)
  {
    char cppfile_path[255];
    sprintf(cppfile_path, "%s/main.cpp", folder_path);
    file = fopen(cppfile_path, "w");
    if (!file) // Проверка на ошибку создания файла
      return 2;
    fprintf(file, "#include <iostream>\n\nint main()\n{\n  std::cout << \"Hello, World!\" << std::endl;\n  return 0;\n}\n");
  }
  else
  {
    char cfile_path[255];
    sprintf(cfile_path, "%s/main.c", folder_path);
    file = fopen(cfile_path, "w");
    if (!file) // Проверка на ошибку создания файла
      return 2;
    fprintf(file, "#include <stdio.h>\n\nint main()\n{\n  printf(\"Hello, World!\\n\");\n  return 0;\n}\n");
  }

  fclose(file);
  return 0;
}

char create_json_config(char *folder_path[256], char is_cpp)
{
  cJSON *root = cJSON_CreateObject();
  if (is_cpp)
    cJSON_AddStringToObject(root, "language", "cpp");
  else
    cJSON_AddStringToObject(root, "language", "c");

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

char new_module()
{
  char cwd[256];
  getcwd(cwd, 256); // получить текущую рабочую директорию
  char json_path[256];
  sprintf(json_path, "%s/ccp.json", cwd);
  FILE *config_file = fopen(json_path, "r");

  if (!config_file)
    return 2;

  char buffer[256];
  fread(buffer, 1, sizeof(buffer), json_path);
  fclose(config_file);

  cJSON *json = cJSON_Parse(buffer);
  if (!json)
  {
    cJSON_Delete(json);
    return 2;
  }
  cJSON *language = cJSON_GetObjectItem(json, "language");
  if (cJSON_IsString(language) && !language->valuestring)
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

    char module_name[64];
    printf("Введите имя модуля: ");
    scanf("%s", module_name);

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