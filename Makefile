CC=gcc
CFLAGS=-I./headers
DEPS = headers/clients_handling.h headers/clients_registry.h headers/LTE.h headers/preambles.h headers/rrc_connection.h headers/client_ping.h

%.o: %.c $(DEPS)
	$(CC) -pthread -c -o $@ $< $(CFLAGS)

server: src/clients_registry.o src/clients_handling.o src/LTE.o src/client_ping.o src/main.o
	$(CC) -o server src/clients_registry.o src/clients_handling.o src/LTE.o src/client_ping.o src/main.o
