#include "project.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cJSON.h"

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

// Создает проект на CMake
char create_cmake_project(Project *proj)
{
  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd)))
  {
    perror("Ошибка получения текущей директории");
    return 1;
  }

  char folder_path[PATH_MAX * 2];
  char cmakefile_path[PATH_MAX * 2];
  char cfile_path[PATH_MAX * 2];

  // Создаем основную папку проекта
  if ((size_t)snprintf(folder_path, sizeof(folder_path), "%s/%s", cwd, proj->name) >= sizeof(folder_path))
  {
    fprintf(stderr, "Слишком длинный путь к папке проекта\n");
    return 1;
  }
  if (mkdir(folder_path, 0777) != 0 && errno != EEXIST)
  {
    perror("Ошибка создания папки проекта");
    return 1;
  }

  // Создаем папку src
  if ((size_t)snprintf(folder_path, sizeof(folder_path), "%s/%s/src", cwd, proj->name) >= sizeof(folder_path))
  {
    fprintf(stderr, "Слишком длинный путь к папке src\n");
    return 1;
  }
  if (mkdir(folder_path, 0777) != 0 && errno != EEXIST)
  {
    perror("Ошибка создания папки src");
    return 1;
  }

  // Создаем папку include
  if ((size_t)snprintf(folder_path, sizeof(folder_path), "%s/%s/include", cwd, proj->name) >= sizeof(folder_path))
  {
    fprintf(stderr, "Слишком длинный путь к папке include\n");
    return 1;
  }
  if (mkdir(folder_path, 0777) != 0 && errno != EEXIST)
  {
    perror("Ошибка создания папки include");
    return 1;
  }

  snprintf(folder_path, sizeof(folder_path), "%s/%s", cwd, proj->name);

  // Создаем CMakeLists.txt
  if ((size_t)snprintf(cmakefile_path, sizeof(cmakefile_path), "%s/CMakeLists.txt", folder_path) >= sizeof(cmakefile_path))
  {
    fprintf(stderr, "Слишком длинный путь к CMakeLists.txt\n");
    return 1;
  }
  FILE *file = fopen(cmakefile_path, "w");
  if (!file)
  {
    perror("Ошибка создания CMakeLists.txt");
    return 2;
  }

  fprintf(file, "cmake_minimum_required(VERSION 4.0.0)\n\n");
  fprintf(file, "project(%s VERSION 0.1.0)\n\n", proj->name);
  fprintf(file, "file(GLOB SOURCES src/*.%s)\n\n", proj->lang_ext);
  fprintf(file, "add_executable(${PROJECT_NAME} ${SOURCES})\n");
  fprintf(file, "target_include_directories(${PROJECT_NAME} PRIVATE include)\n");
  fclose(file);

  // Создаем основной файл
  if ((size_t)snprintf(cfile_path, sizeof(cfile_path), "%s/src/main.%s", folder_path, proj->lang_ext) >= sizeof(cfile_path))
  {
    fprintf(stderr, "Слишком длинный путь к основному файлу\n");
    return 1;
  }
  file = fopen(cfile_path, "w");
  if (!file)
  {
    perror("Ошибка создания основного файла");
    return 2;
  }
  fprintf(file, "int main()\n{\n\treturn 0;\n}\n");
  fclose(file);

  if (create_json_config(folder_path, proj->lang_ext, proj->header_ext) != 0)
  {
    perror("Ошибка создания конфига");
    return 3;
  }

  return 0;
}

// Создает проект на Makefile
char create_make_project(Project *proj)
{
  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd)))
  {
    perror("Ошибка получения текущей директории");
    return 1;
  }

  char folder_path[PATH_MAX * 2];
  char makefile_path[PATH_MAX * 2];
  char cfile_path[PATH_MAX * 2];

  // Создаем основную папку проекта
  if ((size_t)snprintf(folder_path, sizeof(folder_path), "%s/%s", cwd, proj->name) >= sizeof(folder_path))
  {
    fprintf(stderr, "Слишком длинный путь к папке проекта\n");
    return 1;
  }
  if (mkdir(folder_path, 0777) != 0 && errno != EEXIST)
  {
    perror("Ошибка создания папки проекта");
    return 1;
  }

  // Создаем папку src
  if ((size_t)snprintf(folder_path, sizeof(folder_path), "%s/%s/src", cwd, proj->name) >= sizeof(folder_path))
  {
    fprintf(stderr, "Слишком длинный путь к папке src\n");
    return 1;
  }
  if (mkdir(folder_path, 0777) != 0 && errno != EEXIST)
  {
    perror("Ошибка создания папки src");
    return 1;
  }

  // Создаем папку include
  if ((size_t)snprintf(folder_path, sizeof(folder_path), "%s/%s/include", cwd, proj->name) >= sizeof(folder_path))
  {
    fprintf(stderr, "Слишком длинный путь к папке include\n");
    return 1;
  }
  if (mkdir(folder_path, 0777) != 0 && errno != EEXIST)
  {
    perror("Ошибка создания папки include");
    return 1;
  }

  snprintf(folder_path, sizeof(folder_path), "%s/%s", cwd, proj->name);

  // Создаем Makefile
  if ((size_t)snprintf(makefile_path, sizeof(makefile_path), "%s/Makefile", folder_path) >= sizeof(makefile_path))
  {
    fprintf(stderr, "Слишком длинный путь к Makefile\n");
    return 1;
  }
  FILE *file = fopen(makefile_path, "w");
  if (!file)
  {
    perror("Ошибка создания Makefile");
    return 2;
  }

  fprintf(file, "TARGET=%s\n", proj->name);
  fprintf(file, "CC=%s\n", proj->compiler);
  fprintf(file, "FLAGS=-Wall -Wextra -Werror -g -Iinclude\n");
  fprintf(file, "SOURCES=$(wildcard src/*.%s)\n", proj->lang_ext);
  fprintf(file, "OBJECTS=$(SOURCES:.%s=.o)\n", proj->lang_ext);

  fprintf(file, "\nall: $(TARGET)\n");
  fprintf(file, "\n$(TARGET): $(OBJECTS)\n");
  fprintf(file, "\t$(CC) $(FLAGS) -o $@ $^\n");
  fprintf(file, "\n%%.o: %%.%s\n", proj->lang_ext);
  fprintf(file, "\t$(CC) $(FLAGS) -c $< -o $@\n");
  fprintf(file, "\nclean:\n");
#ifdef _WIN32
  fprintf(file, "\trd /s /q $(OBJECTS) $(TARGET).exe\n");
#else
  fprintf(file, "\trm -f $(OBJECTS) $(TARGET)\n");
#endif

  fclose(file);

  // Создаем основной файл
  if ((size_t)snprintf(cfile_path, sizeof(cfile_path), "%s/src/main.%s", folder_path, proj->lang_ext) >= sizeof(cfile_path))
  {
    fprintf(stderr, "Слишком длинный путь к основному файлу\n");
    return 1;
  }
  file = fopen(cfile_path, "w");
  if (!file)
  {
    perror("Ошибка создания основного файла");
    return 2;
  }
  fprintf(file, "int main(i)\n{\n\treturn 0;\n}\n");
  fclose(file);

  if (create_json_config(folder_path, proj->lang_ext, proj->header_ext) != 0)
  {
    perror("Ошибка создания конфига");
    return 3;
  }

  return 0;
}