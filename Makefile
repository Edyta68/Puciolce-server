CC=gcc
CFLAGS=-I./headers
DEPS = headers/clients_handling.h headers/clients_registry.h headers/LTE.h headers/preambles.h headers/rrc_connection.h


server: src/clients_registry.o src/clients_handling.o src/LTE.o src/main.o
	$(CC) -o server src/clients_registry.o src/clients_handling.o src/LTE.o src/main.o
