# Менеджер по созданию С\С++ приложения CreateCProject

## Установка
### С помощью Git
```bash
git clone https://github.com/yuriyProgg/CreateCProject
cd CreateCProject
make
```

Добавте папку в PATH

## Использование
### Для Windows
```cmd
.\ccp.exe <Имя проекта> [Флаги]
```
### Для Linux\MacOS
```bash
./ccp <Имя проекта> [Флаги]
```

### Флаги
- -cpp: Использовать C++. По умолчанию используется C.
- -std=<ЗНАЧЕНИЕ>: Стандарт С\С++. По умолчанию 23.
- -cmake: Использовать CMake. По умолчанию используется Makefile.
- -h, --help: Показать этот текст.
