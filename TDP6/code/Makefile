CC=gcc
CFLAGS=-g -std=gnu99 -Wall
LDFLAGS=-fopenmp -lpthread

all: sequential openmp pthread

sequential: life_seq.c
	$(CC) $(CFLAGS) -o life_seq life_seq.c

openmp: life_omp.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o life_omp life_omp.c

pthread: life_pthread.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o life_pthread life_pthread.c

clean:
	rm life_seq life_omp life_pthread 2> /dev/null
