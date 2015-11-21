CC=mpicc
CFLAGS=-g -std=gnu99 -Wall
LDFLAGS=-lm
OBJ= $(SRC:.c=.o)

all: driver

driver: driver.c mpi_fox.o util.o
	$(CC) $(CFLAGS) -o driver driver.c mpi_fox.o util.o

mpi_fox.o: mpi_fox.c mpi_fox.h util.o
util.o: util.c util.h 

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

check-syntax:
	$(CC) -o nul $(CFLAGS) -S ${CHK_SOURCES}

