#pragma once

char create_cmake_project(char *project_name, char is_cpp, char *std);
char create_make_project(char *project_name, char is_cpp, char *std);
char create_json_config(char *folder_path[256], char is_cpp);
