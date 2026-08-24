
CC=gcc

CFLAGS=-O2 -Wall -Wextra -I..

LIBS=-L. -ldtypes 

TARGET=libdtypes.a

SOURCES=$(wildcard *.c)

OBJECTS=$(SOURCES:.c=.o)

.PHONY: build

build: $(TARGET)

$(TARGET): $(OBJECTS)
	ar rcs libdtypes.a $(OBJECTS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY: clean

clean:
	rm *.o
