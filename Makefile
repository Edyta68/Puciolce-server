CC = gcc
CC_FLAGS = -I./headers
#CFLAGS = -Wall -g

SOURCES = $(wildcard ./src/*.c)
OBJECTS = $(SOURCES:.c=.o)
DEPS = $(wildcard ./headers/*.h)
EXEC = server

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -lpthread -o $(EXEC)

%.o: %.c $(DEPS)
	$(CC) -c $(CC_FLAGS) $< -o $@

valgrind:
	valgrind ./server $(PORT)
