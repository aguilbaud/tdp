#include <mpi.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "util.h"

#define NB_ITER 2

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
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int n;
    char * exec_path = "data/test2.univ";
    FILE *fp = fopen(exec_path,"r");
    if(fp == NULL){
	printf("Problem opening file:%s\n",exec_path);
	MPI_Finalize();
	return EXIT_FAILURE;
    }

    particle_t *p = load_file(fp, &n);
    if(p == NULL){
	MPI_Finalize();
	return EXIT_FAILURE;
    }
    int p_beg = (n/size)*rank;
    int p_end = (n/size)*(rank+1);
    int loc_size = n/size;
    printf("n:%d loc_size %d\n", n,loc_size);

    
    /*Init mpi datatypes*/
    MPI_Datatype MPI_PARTICLE;
    init_mpi_datatype(&MPI_PARTICLE, p);
    MPI_Type_commit(&MPI_PARTICLE);
            
    /*Determining our neighbours*/
    int next = (rank + 1)%size;
    int prev = ((rank - 1)+size)%size;
    
    double my_dt = 100000;
    
    particle_t loc_p; 
    particle_t *buf_p;
    buf_p = malloc(loc_size*sizeof(particle_t));
    //memcpy(loc_p, p, loc_size);
    //memcpy(loc_p, p, loc_size);

    char dest[23];
    sprintf(dest, "output/outputmpi%2.2d.dat", rank);
    
    
    printf("coucou\n");
    MPI_Request req_send, req_recv;
    MPI_Status stat_send, stat_recv;
    for(int i = 0; i < NB_ITER; i++){
	/*To store the computed acceleration of our particles*/
	double *acc = malloc(loc_size*2*sizeof(double));
	if(acc == NULL){
	    fprintf(stderr, "Problem allocating space\n");
	    free(p);
	    MPI_Finalize();
	    return EXIT_FAILURE;
	}
	
	double min_dt;
	for (int j = 0; j < size; j++) {	        
	    //Create persistent communication request
	    MPI_Send_init(p + p_beg, loc_size, MPI_PARTICLE, next, 0, MPI_COMM_WORLD, &req_send);
	    //Si n/size ne tombe pas juste modifier loc_size
	    MPI_Recv_init(buf_p, loc_size, MPI_PARTICLE, prev, 0, MPI_COMM_WORLD, &req_recv);

	    MPI_Start(&req_send);
	    MPI_Start(&req_recv);

	    //Peut etre mettre  += pour la maj de acc
	    forces2(p + p_beg, loc_size, buf_p, loc_size, acc, &min_dt, (j==rank));
	    if(min_dt < my_dt)
		my_dt = min_dt;
	
	    printf("%d -> min_dt = %f\n", rank, min_dt);
	    

	    MPI_Wait(&req_send, &stat_send);
	    MPI_Wait(&req_recv, &stat_recv);
	}
	
	
	double global_dt;
	MPI_Reduce_local(&my_dt, &global_dt, 1, MPI_DOUBLE, MPI_MIN);
	update_pos_vel(p + p_beg, loc_size, acc, my_dt);
	write_plot(dest, loc_size, p + p_beg);
	free(acc);
    }
    
    
    free(p);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

