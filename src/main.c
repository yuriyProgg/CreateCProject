#include <stdio.h>
#include <string.h>

#include "project.h"
#include "module.h"
#include "config.h"

int main(int argc, char *argv[])
{
  if (argc <= 1 || !strncmp(argv[1], "-h", 2) || !strncmp(argv[1], "--help", 6))
  {
    printf("Используете `%s <Имя проекта или Команда> [Флаги]`.\n", argv[0]);
    puts("\nКоманды:");
    puts("  mod <Имя модуля>\tСоздать новый модуль в директории указанные в ccp.json");
    puts("\nФлаги (используются при создании проекта): ");
    puts("  -cpp, -cc\t\tИспользовать C++ (.cpp или .cc). По умолчанию используется C.");
    puts("  -hpp\t\t\tИспользовать заголовочные файлы `.hpp`.");
    puts("  -cmake\t\tИспользовать CMake. По умолчанию используется Makefile.");
    puts("  -h, --help\t\tПоказать этот текст.");
    return 1;
  }

  if (!strncmp(argv[1], "mod", 3))
  {
    if (argv[2] == NULL)
    {
      printf("Не указано имя модуля\n");
      return 1;
    }
    char result = new_module(argv[2]);
    if (result == 0)
      printf("Модуль `%s` создан\n", argv[2]);
    else if (result == 2)
      printf("Ошибка создания файла\n");
    else if (result != 0)
      printf("Ошибка №%d\n", result);
    return result;
  }

  // Иницилизация переменных
  Project proj;
  char *project_name = argv[1], *tool = "make", *lang_ext = "c", *header_ext = "h";
  proj.compiler = "gcc";

  // Ищем дполнительные флаги
  for (int i = 1; i < argc; ++i)
  {
    if (!strncmp(argv[i], "-cmake", 6))
      tool = "cmake";
    if (!strncmp(argv[i], "-cpp", 4))
    {
      lang_ext = "cpp";
      proj.compiler = "g++";
    }
    if (!strncmp(argv[i], "-cc", 3))
    {
      lang_ext = "cc";
      proj.compiler = "g++";
    }
    if (!strncmp(argv[i], "-hpp", 4))
      header_ext = "hpp";
  }

  // Создаем проект
  proj.name = project_name;
  proj.lang_ext = lang_ext;
  proj.header_ext = header_ext;

  char result = !strncmp(tool, "make", 4) ? create_make_project(&proj) : create_cmake_project(&proj);
  if (result == 2)
    puts("Ошибка создания файла");
  else if (result != 0)
    printf("Ошибка №%d\n", result);
  printf("Проект `%s` создан\n", project_name);
  return result;
}