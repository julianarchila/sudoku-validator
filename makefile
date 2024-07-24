SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)

CC = gcc
CFLAGS = -Wall -g
LDFLAGS = 

.PHONY: all clean

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o main.out $(LDFLAGS)

clean:
	rm -f $(OBJECTS) main

run: all
	./main.out

