CC=mpicc
CFLAGS=-O2 -std=gnu99 -Wall -I${MKLROOT}/include -ldl
LDFLAGS=-lm
OBJ= $(SRC:.c=.o)
MKL_LINK = -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_core.a ${MKLROOT}/lib/intel64/libmkl_sequential.a -Wl,--end-group -lpthread -lm


all: driver timing gen_mat sequential

timing: timing.c mpi_fox.o dgemm.o util.o
	$(CC) $(CFLAGS) -o timing timing.c mpi_fox.o dgemm.o util.o $(LDFLAGS) $(MKL_LINK)

driver: driver.c mpi_fox.o dgemm.o util.o
	$(CC) $(CFLAGS) -o driver driver.c mpi_fox.o dgemm.o util.o $(LDFLAGS) $(MKL_LINK)

sequential:
	$(CC) $(CFLAGS) -o sequential sequential.c mpi_fox.o util.o $(LDFLAGS) $(MKL_LINK)

gen_mat: gen_mat.c
	$(CC) $(CFLAGS) -o gen_mat gen_mat.c


mpi_fox.o: mpi_fox.c mpi_fox.h util.o
util.o: util.c util.h 
dgemm.o: dgemm.c mycblas.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) 
clean:
	rm *.o driver gen_mat timing sequential 
