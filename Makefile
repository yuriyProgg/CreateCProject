CC=clang
FLAGS=-O3

TARGET=ccp
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) -o $@ $^

%.o: %.c
	$(CC) $(FLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)
