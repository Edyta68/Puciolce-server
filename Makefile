CC = gcc
CC_FLAGS = -I./headers

SOURCES = $(wildcard ./src/*.c)
OBJECTS = $(SOURCES:.c=.o)
DEPS = $(wildcard ./headers/*.h)
EXEC = server

$(EXEC): $(OBJECTS)
	$(CC) -pthread $(OBJECTS) -o $(EXEC)

%.o: %.c $(DEPS)
	$(CC) -c $(CC_FLAGS) $< -o $@
