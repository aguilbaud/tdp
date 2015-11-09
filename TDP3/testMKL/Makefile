CC = gcc
CFLAGS = -O2 -std=gnu99 -Wall -I${MKLROOT}/include -ldl
MKL_LINK = -Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a ${MKLROOT}/lib/intel64/libmkl_core.a ${MKLROOT}/lib/intel64/libmkl_sequential.a -Wl,--end-group -lpthread -lm

main: dgetrf.c ../util.o ../dgetf2.o ../perf.o
	$(CC) $(CFLAGS) -o main dgetrf.c ../util.o ../dgetf2.o ../perf.o $(MKL_LINK)
