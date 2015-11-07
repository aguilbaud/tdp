CC = gcc
CFLAGS =   -std=gnu99 -Wall 

MPI = mpicc

all: main timing

main: main.c util.o dgetf2.o dgemm.o
	$(CC) $(CFLAGS) -o main main.c util.o dgetf2.o dgemm.o -lm

timing: timing.c dgetf2.o util.o perf.o
	$(CC) $(CFLAGS) -o timing timing.c dgetf2.o util.o perf.o

perf.o: perf.c perf.h
	$(CC) $(CFLAGS) -c perf.c

dgemm.o: dgemm.c dgemm.h
	$(CC) $(CFLAGS) -c dgemm.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

dgetf2.o: dgetf2.h dgetf2.c cblas.h
	$(CC) $(CFLAGS) -c dgetf2.c

clean: 
	rm *.o main timing