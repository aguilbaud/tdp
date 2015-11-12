#include <mpi.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "util.h"

#define DEF_DT 10000

void usage(){
    printf("Usage:\n");
    printf("-i <x>\t\tNombre d'itérations\n");
    printf("-f <file>\tSequential execution\n");
    printf("-p\t\tOutput number of particles and time for performance measurment\n");
    printf("\n");
}


void init_mpi_datatype(MPI_Datatype * type, particle_t * p){
  MPI_Datatype types[2] = {MPI_DOUBLE, MPI_DOUBLE};
  int blocklen[2]   = {1, 2};
  MPI_Aint i1, i2;
  MPI_Aint disp[2];
  
    MPI_Get_address(&p[0], &i1);
    MPI_Get_address(&p[0].m, &i2); disp[0] = i2 - i1;
    MPI_Get_address(&p[0].p, &i2); disp[1] = i2 - i1;
    //    MPI_Get_address(&p[0].v, &i2); disp[2] = i2 - i1;

    MPI_Type_create_struct(2
, blocklen, disp, types, type);
}


int main(int argc, char *argv[]){
    if(argc == 1){
	usage();
	return EXIT_FAILURE;
    }

    int rank, size;
    //MPI initialisation
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Default values
    int nb_iter = 20;
       
    //Execution flags
    int p_flag = 0;
    char *exec_path = NULL;
    int c;
    opterr = 0;
   
    while ((c = getopt (argc, argv, "i:f:p")) != -1)
	switch (c) {
	case 'i':
	    nb_iter = atoi(optarg);
	    break;
	case 'f':
	    exec_path = optarg;
	    break;
	case 'p':
	    p_flag = 1;
	    break;
	case '?':
	    if(rank == 0){
		fprintf (stderr, "Option -%c requires an argument.\n", optopt);
		usage();
	    }
	    MPI_Finalize();
	    return EXIT_FAILURE;
	    break;
	default:
	    if(rank == 0)
		usage();
	    MPI_Finalize();
	    return EXIT_FAILURE;
	    break;
	}

    char dest[23];
    FILE *fp, *output_data;
    int n;
    
    //Creating the ouptut file of this process
    sprintf(dest, "output/outputmpi%2.2d.dat", rank);
    output_data = fopen(dest,"w+");
    if(output_data == NULL){
	printf("Problem opening file:%s\n",dest);
	MPI_Finalize();
	return EXIT_FAILURE;
    }

    //Loading the file of the simulation and creating the global particle set
    fp = fopen(exec_path,"r");
    if(fp == NULL){
	printf("Problem opening file:%s\n",exec_path);
	MPI_Finalize();
	return EXIT_FAILURE;
    }
    particle_t *p = load_file(fp, &n);
    fclose(fp);
    if(p == NULL){
	MPI_Finalize();
	return EXIT_FAILURE;
    }
    if(p == NULL){
	MPI_Finalize();
	return EXIT_FAILURE;
    }
    
    //Determining the size of the local buffer
    if(n<size){
	if(rank==0)
	    fprintf(stderr, "The number of processes must be greater than the number of particles");
	free(p);
	MPI_Finalize();
	return EXIT_FAILURE;
    }
    int buf_size = ceil(n/(double)size);//(n%size) ? (n+size)/size : (n/size);
    int loc_size = buf_size;
    if(rank == size -1){
	loc_size = (buf_size - (buf_size*size - n));
    }
    if(loc_size <= 0){
      fprintf(stderr, "There is too much threads for this problem. One thread will not be used");
      
      MPI_Abort(MPI_COMM_WORLD,0);
      return EXIT_SUCCESS;
    }    
    
    particle_t *loc_p  = malloc(buf_size*sizeof(particle_t));//The local particle set
    particle_t *comm_p = malloc(buf_size*sizeof(particle_t));//A communication buffer
    particle_t *calc_p = malloc(buf_size*sizeof(particle_t));//A buffer used for calculation

    //Each process get his part of the particle set
    int p_beg = buf_size*rank;
    memcpy(loc_p, p + p_beg, loc_size*sizeof(particle_t));
    free(p);


    //Init mpi datatypes
    MPI_Datatype MPI_PARTICLE;
    init_mpi_datatype(&MPI_PARTICLE, p);
    MPI_Type_commit(&MPI_PARTICLE);
            
    //Determining our neighbours
    int next = (rank + 1)%size;
    int prev = ((rank - 1)+size)%size;
    
    double my_dt = DEF_DT;
    int rate = nb_iter/1000;//output gif, if specified, will contain 1000 pictures
    
    //Initialising persistent requets
    MPI_Request req_send, req_recv;
    MPI_Status stat_send, stat_recv;
    MPI_Recv_init(comm_p, buf_size, MPI_PARTICLE, prev, 0, MPI_COMM_WORLD, &req_recv);
    MPI_Send_init(calc_p, buf_size, MPI_PARTICLE, next, 0, MPI_COMM_WORLD, &req_send);
	
    if(!p_flag && rank == 0)
	printf("%d particles\n",n);
    
    struct timeval start, end;
    double elapsed = 0.0;

    gettimeofday(&start,NULL);
    for(int i = 0; i < nb_iter; i++){ 
	/*To store the computed acceleration of our particles*/
	double *acc = calloc(loc_size*2,sizeof(double));
	
	if(acc == NULL){ 
	    fprintf(stderr, "Problem during allocation\n");
	    free(loc_p);
	    free(comm_p);
	    free(calc_p);
	    MPI_Finalize();
	    return EXIT_FAILURE;
	} 
	 
	double min_dt = DBL_MAX;
	memcpy(calc_p, loc_p, buf_size*sizeof(particle_t));
	for (int j = 0; j < size; j++) {
	  MPI_Start(&req_send);
	    MPI_Start(&req_recv);
	  
	    forces(loc_p, loc_size, calc_p, buf_size, acc, &min_dt, (j==0));
	    if(min_dt < my_dt)
		my_dt = min_dt;
		
	    MPI_Wait(&req_send, &stat_send);
	    MPI_Wait(&req_recv, &stat_recv);
	    memcpy(calc_p, comm_p, buf_size * sizeof(particle_t));
	}
	
	double global_dt;
	MPI_Allreduce(&my_dt, &global_dt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
	update_pos_vel(loc_p, loc_size, acc, global_dt);
	
	if(!p_flag){
	    if(rate == 0){
		write_plot(output_data, loc_size, loc_p);
	    }
	    else{
		if(((i+1)%rate) == 1)
		    write_plot(output_data, loc_size, loc_p);
	    }
	}
	
	free(acc);
    } 
    gettimeofday(&end,NULL);
    elapsed = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsed += (end.tv_usec - start.tv_usec) / 1000.0;
    double global_time;
    MPI_Reduce(&elapsed, &global_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    global_time = global_time / size;
    if(p_flag && rank == 0)
	printf("%d %f\n", n, global_time/nb_iter);
    
    
    free(loc_p);
    free(comm_p);
    free(calc_p);
    MPI_Request_free(&req_recv);
    MPI_Request_free(&req_send);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

