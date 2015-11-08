CC = gcc
CFLAGS = -g -std=gnu99 -Wall

MPI = mpicc

all: main timing


main: main.c util.o dgetf2.o dgemm.o dtrsv.o dtrsm.o dgesv.o dgetrf.o dscal.o dger.o 
	$(CC) $(CFLAGS) -o main main.c util.o dgemm.o dtrsv.o dtrsm.o dgesv.o dgetrf.o dscal.o dger.o dgetf2.o -lm

timing: timing.c dgetf2.o util.o perf.o dscal.o dger.o
	$(CC) $(CFLAGS) -o timing timing.c dgetf2.o util.o perf.o dscal.o dger.o


dgemm.o: dgemm.c mycblas.h cblas.h
dger.o: dger.c mycblas.h cblas.h
dgesv.o: dgesv.c mycblas.h cblas.h dgetrf.c dtrsm.c
dgetf2.o: dgetf2.c mycblas.h cblas.h dscal.c dger.c
dgetrf.o: dgetrf.c mycblas.h cblas.h dgetf2.c dtrsm.c
dscal.o: dscal.c mycblas.h cblas.h dgemm.c
dtrsm.o: dtrsm.c mycblas.h cblas.h dtrsv.c
dtrsv.o: dtrsv.c mycblas.h cblas.h

perf.o: perf.c perf.h
util.o: util.c


clean: 
	rm *.o main timing
