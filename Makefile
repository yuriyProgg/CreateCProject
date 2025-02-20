CC=gcc
FLAGS=-w -O3

TARGET=ccp
INCLUDES=-I.
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(INCLUDES) -o $@ $^

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDES) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)
