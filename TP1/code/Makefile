CC = gcc
CFLAGS = -g -std=c99 -Wall

all: timing driver

timing:	timing.c dgemm.o ddot.o	util.o perf.o
	$(CC) $(CFLAGS) -o timing timing.c dgemm.o ddot.o util.o perf.o

driver: driver.c queue.h dgemm.o ddot.o util.o
	$(CC) $(CFLAGS) -pthread -o driver driver.c dgemm.o ddot.o util.o


perf.o: perf/perf.c perf/perf.h
	$(CC) $(CFLAGS) -c perf/perf.c

dgemm.o: dgemm.c dgemm.h
	$(CC) $(CFLAGS) -c dgemm.c

ddot.o: ddot.c cblas.h
	$(CC) $(CFLAGS) -c ddot.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean: 
	rm *.o driver timing

