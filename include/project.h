#pragma once

#define PATH_MAX 256

typedef struct
{
  const char *name;
  const char *lang_ext;
  const char *header_ext;
  const char *compiler; // Нужен только для Makefile
} Project;

char create_cmake_project(Project *proj);
char create_make_project(Project *proj);