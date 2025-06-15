CC = gcc
FLAGS = -O2 -std=c23 -flto=auto -pie -s -DNDEBUG # Release mode
# FLAGS = -g -Wall -Wextra -Wpedantic -std=c23 # Debug mode 
LDFLAGS = -flto=auto -pie -s

TARGET = ccp
INCLUDES = -Iinclude -Ilib
SOURCES = $(wildcard src/*.c) $(wildcard lib/*.c)
OBJECTS = $(SOURCES:src/%.c=build/%.o)

# Проверка, используется ли MinGW
ifneq ($(OS),Windows_NT)
    ifeq ($(findstring mingw,$(CC)),mingw)
        LDFLAGS += -lmingw32 -lmsvcrt
    endif
else
    LDFLAGS += -static
endif

all: build/$(TARGET)

build/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean

clean:
	rm -rf build $(TARGET)
