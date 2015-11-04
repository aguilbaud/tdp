#include <mpi.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <float.h>
#include "util.h"

#define NB_ITER 10000

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
    char * exec_path = "data/junk.univ";
    FILE *fp = fopen(exec_path,"r");
    if(fp == NULL){
	printf("Problem opening file:%s\n",exec_path);
	MPI_Finalize();
	return EXIT_FAILURE;
    }
    
    char dest[23];
    sprintf(dest, "output/outputmpi%2.2d.dat", rank);
    FILE *output_data = fopen(dest,"w+");
    if(output_data == NULL){
	printf("Problem opening file:%s\n",dest);
	MPI_Finalize();
	return EXIT_FAILURE;
    }

    particle_t *p = load_file(fp, &n);
    printf("debut n: %d\n", n);
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
    
    particle_t *loc_p; 
    particle_t *comm_p;
    particle_t *calc_p;
    comm_p = malloc(loc_size*sizeof(particle_t));
    loc_p = malloc(loc_size*sizeof(particle_t));
    calc_p = malloc(loc_size*sizeof(particle_t));
    memcpy(loc_p, p + p_beg, loc_size*sizeof(particle_t));
        
    int rate = NB_ITER/1000;//output gif, if specified, will contain 1000 pictures
    
    MPI_Request req_send, req_recv;
    MPI_Status stat_send, stat_recv;
    for(int i = 0; i < NB_ITER; i++){
	/*To store the computed acceleration of our particles*/
	double *acc = calloc(loc_size*2,sizeof(double));
	if(acc == NULL){
	    fprintf(stderr, "Problem allocating space\n");
	    free(p);
	    MPI_Finalize();
	    return EXIT_FAILURE;
	}
	
	double min_dt = DBL_MAX;
	memcpy(calc_p, loc_p, loc_size*sizeof(particle_t));
	for (int j = 0; j < size; j++) {
	    
	    if(j%2 == 0){
		MPI_Recv_init(comm_p, loc_size, MPI_PARTICLE, prev, 0, MPI_COMM_WORLD, &req_recv);
		MPI_Send_init(calc_p, loc_size, MPI_PARTICLE, next, 0, MPI_COMM_WORLD, &req_send);
		
		MPI_Start(&req_send);
		MPI_Start(&req_recv);

		forces2(loc_p, loc_size, calc_p, loc_size, acc, &min_dt, (j==0));
		if(min_dt < my_dt)
		    my_dt = min_dt;
		
		MPI_Wait(&req_send, &stat_send);
		MPI_Wait(&req_recv, &stat_recv);
	    }
	    else {
		//Si n/size ne tombe pas juste modifier loc_size
		MPI_Recv_init(calc_p, loc_size, MPI_PARTICLE, prev, 0, MPI_COMM_WORLD, &req_recv);
		//Create persistent communication request
		MPI_Send_init(comm_p, loc_size, MPI_PARTICLE, next, 0, MPI_COMM_WORLD, &req_send);
		
		MPI_Start(&req_send);
		MPI_Start(&req_recv);

		forces2(loc_p, loc_size, comm_p, loc_size, acc, &min_dt, (j==0));
		if(min_dt < my_dt)
		    my_dt = min_dt;
	
		// printf("%d -> min_dt = %f\n", rank, min_dt);
	    

		MPI_Wait(&req_send, &stat_send);
		MPI_Wait(&req_recv, &stat_recv);
	    }
	
	}
	double global_dt;
	MPI_Allreduce(&my_dt, &global_dt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
	update_pos_vel(loc_p, loc_size, acc, global_dt);
	
	if(rate == 0){
	    write_plot(output_data, loc_size, loc_p); 
	}
	else{
	  if(((i+1)%rate) == 1)
	    write_plot(output_data, loc_size, loc_p); 
	}
	free(acc);
    }
    
    
    free(p);
    free(loc_p);
    free(comm_p);
    free(calc_p);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

