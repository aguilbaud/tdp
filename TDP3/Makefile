CC = gcc
CFLAGS = -g -std=gnu99 -Wall 

MPI = mpicc

all: main

main: main.c util.o dgetf2.o
	$(CC) $(CFLAGS) -o main main.c util.o dgetf2.o


util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

dgetf2.o: dgetf2.h cblas.h
	$(CC) $(CFLAGS) -c dgetf2.c
