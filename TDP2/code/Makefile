CC = gcc
CFLAGS = -g -std=c99 -Wall

MPI = mpicc


all: driver

driver: driver.c particle.o util.o
	$(MPI) $(CFLAGS) -o driver driver.c particle.o util.o -lm

particle.o: particle.c particle.h
	$(CC) $(CFLAGS) -c particle.c -lm

# perf.o: perf/perf.c perf/perf.h
# 	$(CC) $(CFLAGS) -c perf/perf.c

# dgemm.o: dgemm.c dgemm.h
# 	$(CC) $(CFLAGS) -c dgemm.c

# ddot.o: ddot.c cblas.h
# 	$(CC) $(CFLAGS) -c ddot.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean: 
	rm *.o driver output/*

