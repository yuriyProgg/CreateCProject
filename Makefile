CC=gcc
FLAGS=-w -O3
LDFLAGS=

TARGET=ccp
INCLUDES=-I.
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

# Проверка, используется ли MinGW
ifneq ($(HOST),)
    ifeq ($(findstring mingw, $(HOST)), mingw)
        CC=x86_64-w64-mingw32-gcc
        LDFLAGS+=-lmingw32 -lmsvcrt
    endif
endif
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDES) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)
