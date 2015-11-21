CC=mpicc
CFLAGS=-O2 -std=gnu99 -Wall
LDFLAGS=-lm
MPI = mpicc
OBJ= $(SRC:.c=.o)


all: driver timing driver_mpi timing_mpi

driver_mpi: driver_mpi.c dgesv_mpi.o util.o dgetf2.o dger.o dscal.o dtrsv.o dgemm.o
	$(CC) $(CFLAGS) -o driver_mpi driver_mpi.c dgesv_mpi.o util.c dgetf2.o dger.o dscal.o dtrsm.o dtrsv.o dgemm.o -lm 

driver: driver.c util.o dgetf2.o dgemm.o dtrsv.o dtrsm.o dgesv.o dgetrf.o dscal.o dger.o 
	$(CC) $(CFLAGS) -o driver driver.c util.o dgemm.o dtrsv.o dtrsm.o dgesv.o dgetrf.o dscal.o dger.o dgetf2.o -lm

timing: timing.c dgetf2.o util.o perf.o dscal.o dger.o dgetrf.o dtrsm.o dgemm.o dtrsv.o
	$(CC) $(CFLAGS) -o timing timing.c dgetf2.o util.o perf.o dscal.o dger.o dtrsv.o dgetrf.o dtrsm.o dgemm.o

timing_mpi: timing_mpi.c dgesv_mpi.o util.o dgetf2.o dger.o dscal.o dtrsv.o dgemm.o
	$(CC) $(CFLAGS) -o timing_mpi timing_mpi.c dgesv_mpi.o util.c dgetf2.o dger.o dscal.o dtrsm.o dtrsv.o dgemm.o -lm 


dgesv_mpi.o: dgesv_mpi.c mycblas.h dgetf2.c dtrsm.c dger.c dscal.c dtrsv.c dgemm.c 
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

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

check-syntax:
	$(MPI) -o nul $(CFLAGS) -S ${CHK_SOURCES}

clean: 
	rm *.o driver timing driver_mpi timing_mpi


