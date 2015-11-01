CC = gcc
CFLAGS = -g -std=gnu99 -Wall -O3

MPI = mpicc


all: driver driver_mpi

driver_mpi: driver_mpi.c particle.o util.o dt_calc.o
	$(MPI) $(CFLAGS) -o driver_mpi driver_mpi.c particle.o util.o dt_calc.o -lm

driver: driver.c particle.o util.o dt_calc.o
	$(CC) $(CFLAGS) -o driver driver.c particle.o util.o dt_calc.o -lm

particle.o: particle.c particle.h dt_calc.o
	$(CC) $(CFLAGS) -c particle.c -lm 

dt_calc.o: dt_calc.c dt_calc.h
	$(CC) $(CFLAGS) -c dt_calc.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean_output:
	rm output/* *.gif *.gp *.dat *.png
clean: 
	rm *.o driver driver_mpi output/* *.gif *.gp *.dat 2> /dev/null


