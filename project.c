#include "project.h"

#include <stdio.h>

#include "cJSON.h"

#ifdef __MINGW32__
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

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
