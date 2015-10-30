CC = gcc
CFLAGS = -g -std=gnu99 -Wall

MPI = mpicc


all: driver

driver: driver.c particle.o util.o dt_calc.o
	$(MPI) $(CFLAGS) -o driver driver.c particle.o util.o dt_calc.o -lm

particle.o: particle.c particle.h dt_calc.o
	$(CC) $(CFLAGS) -c particle.c -lm 

dt_calc.o: dt_calc.c dt_calc.h
	$(CC) $(CFLAGS) -c dt_calc.c


# perf.o: perf/perf.c perf/perf.h
# 	$(CC) $(CFLAGS) -c perf/perf.c

# ddot.o: ddot.c cblas.h
# 	$(CC) $(CFLAGS) -c ddot.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean_output:
	rm output/*
clean: 
	rm *.o driver


