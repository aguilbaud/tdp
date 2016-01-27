CC=gcc
MPICC=mpicc
CFLAGS=-g -std=gnu99 -Wall
LDFLAGS=-fopenmp -lpthread -lm

all: sequential openmp pthread mpi_sync


driver: sequential
	$(CC) $(CFLAGS) -o driver driver.c


sequential: life_seq.c
	$(CC) $(CFLAGS) -o life_seq life_seq.c

openmp: life_omp.c
	$(CC) $(CFLAGS) -o life_omp life_omp.c $(LDFLAGS)

pthread: life_pthread.c
	$(CC) $(CFLAGS) -o life_pthread life_pthread.c $(LDFLAGS)

mpi_sync: life_mpi_sync.c
	$(MPICC) $(CFLAGS) -o life_mpi_sync life_mpi_sync.c $(LDFLAGS)

clean:
	rm life_seq life_omp life_pthread life_mpi_sync driver 2> /dev/null
