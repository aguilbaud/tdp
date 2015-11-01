#include <mpi.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "util.h"

void init_mpi_datatype(MPI_Datatype * type, particle_t * p){
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    int blocklen[3]   = {1, 2, 2};
    MPI_Aint i1, i2;
    MPI_Aint disp[3];

    MPI_Get_address(&p[0], &i1);
    MPI_Get_address(&p[0].m, &i2); disp[0] = i2 - i1;
    MPI_Get_address(&p[0].p, &i2); disp[1] = i2 - i1;
    MPI_Get_address(&p[0].v, &i2); disp[2] = i2 - i1;

    MPI_Type_create_struct(3, blocklen, disp, types, type);
}

int main(int argc, char *argv[]){
    int rank, size;
    int n;
    char * exec_path = "data/junk.univ";
    FILE *fp = fopen(exec_path,"r");
    if(fp == NULL){
	printf("Problem opening file:%s\n",exec_path);
	return EXIT_FAILURE;
    }
    particle_t *p = load_file(fp, &n);
    if(p == NULL)
	return EXIT_FAILURE;


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    /*Init mpi datatypes*/
    MPI_Datatype MPI_PARTICLE;
    init_mpi_datatype(&MPI_PARTICLE, p);
    
    MPI_Datatype MPI_PARTICLE_BUFFER;
    MPI_Datatype types[1] = {MPI_PARTICLE};
    int blocklen[1] = {1};
    MPI_Aint i1, i2;
    MPI_Aint disp[1];

    MPI_Get_address(, &i1);
    MPI_Get_address();
        
    /*Determining our neighbours*/
    int next = (rank + 1)%size;
    int prev = ((rank - 1)+size)%size;

    if(rank == 0){
	/*for each processes*/
	/*for each iteration*/
	/*Computing interactions with our own buffer*/
	/*Sending our buffer to our next neighbour*/
	/*Receiving the buffer of our previous neighbour*/

    }
    else{

    }



    MPI_Finalize();
    return EXIT_SUCCESS;
}

