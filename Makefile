CC=gcc
CFLAGS=-Iinclude

exec = server.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -g -Werror -Wall -Wpedantic -MMD -lm -ldl -fPIC -rdynamic -I./include

$(exec): $(objects)
	gcc $(objects) $(flags) -o $(exec)

%.o: %.c %.h
	gcc -c $(flags) $< -o $@

.PHONY: clean

clean:
	-rm $(objects) $(exec)