CC=mpicc
CFLAGS=-g -std=gnu99 -Wall
LDFLAGS=-lm
OBJ= $(SRC:.c=.o)

all: driver gen_mat

driver: driver.c mpi_fox.o dgemm.o util.o
	$(CC) $(CFLAGS) -o driver driver.c mpi_fox.o dgemm.o util.o $(LDFLAGS)

gen_mat: gen_mat.c
	$(CC) $(CFLAGS) -o gen_mat gen_mat.c

mpi_fox.o: mpi_fox.c mpi_fox.h util.o
util.o: util.c util.h 
dgemm.o: dgemm.c mycblas.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm *.o driver gen_mat nul

check-syntax:
	$(CC) -o nul $(CFLAGS) -S ${CHK_SOURCES}

