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
    puts("  mod <Имя модуля>\tСоздать новый модуль (если в проекте есть папки src и include).");
    puts("\nФлаги:");
    puts("  -cpp\t\t\tИспользовать C++. По умолчанию используется C.");
    puts("  -std=<ЗНАЧЕНИЕ>\tСтандарт С\\С++. По умолчанию 23.");
    puts("  -cmake\t\tИспользовать CMake. По умолчанию используется Makefile.");
    puts("  -h, --help\t\tПоказать этот текст.");
    return 1;
  }

  if (argc >= 2 && !strncmp(argv[1], "mod", 3))
  {
    char result = new_module(argv[2]);
    if (result == 0)
      printf("Модуль \"%s\" успешно создан.\n", argv[2]);
    else if (result == 2)
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
  char result = !strncmp(tool, "make", 4) ? create_make_project(project_name, is_cpp, standart) : create_cmake_project(project_name, is_cpp, standart);
  if (result == 2)
    printf("Ошибка создания файла\n");
  else if (result != 0)
    printf("Ошибка №%d\n", result);
  return result;
}