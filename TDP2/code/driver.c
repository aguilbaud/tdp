#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "util.h"

#define DEFAULT_DT 100000

void usage(){
    printf("Usage:\n");
    printf("-i <x>\t\tNombre d'itérations\n");
    printf("-n <x>\t\tNombre de particules\n");
    printf("-g <file>\tGenerate a random pattern\n");
    printf("-f <file>\tSequential execution\n");
    printf("-o <file>\tGenerate a file containing the particles positions\n");
    printf("-p\t\tOutput number of particles and time for performance measurment\n");
    printf("\n");
}


int main_generate(int nb_particle, char *generate_path){
    FILE *fp = fopen(generate_path,"w");
    if(fp == NULL){
	printf("Problem opening file:%s\n",generate_path);
	return EXIT_FAILURE;
    }
    srand(time(NULL));
    particle_t * p = init_universe(nb_particle); 
    write_file(fp, nb_particle, p);
    fclose(fp);
    free(p);
    return EXIT_SUCCESS;
}


int main_seq(int nb_particle, char *exec_path, int nb_iter, char *output_path, int p_flag, int o_flag){

    FILE *fp, *output_data;
    fp = fopen(exec_path,"r");
    if(fp == NULL){
	printf("Problem opening file:%s\n",exec_path);
	return EXIT_FAILURE;
    }
    
    if(o_flag){
	output_data = fopen(output_path, "w+");
	if(output_data == NULL){
	    printf("Problem opening file:%s\n",output_path);
	    return EXIT_FAILURE;
	}
    }

    particle_t *p = load_file(fp, &nb_particle);
    if(p == NULL)
	return EXIT_FAILURE;

    int rate = nb_iter/1000;//output file, if specified, will contain 1000 pictures
    struct timeval start, end;
    double elapsed = 0.0;
       
    if(!p_flag)
	printf("%d particles\n",nb_particle);

    for(int i = 0; i< nb_iter; i++){
	if(!p_flag)
	    printf("\rIter: %d/%d", i+1, nb_iter);
	if(o_flag){
	    if(rate == 0){
		write_plot(output_data, nb_particle, p); 
	    }
	    else
		if(((i+1)%rate) == 1)
		    write_plot(output_data, nb_particle, p); 
	}
	
	gettimeofday(&start,NULL);
	move(p, nb_particle, DEFAULT_DT);
	gettimeofday(&end,NULL);

	elapsed += (end.tv_sec - start.tv_sec) * 1000.0;
	elapsed += (end.tv_usec - start.tv_usec) / 1000.0;
    }
 
    if(p_flag)
	printf("%d %f\n", nb_particle, elapsed);
    else
	printf("\n");

    if(o_flag)
	fclose(output_data);
    fclose(fp);
    free(p);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    //Default values
    int nb_iter = 20;
    int nb_particle = 50;
   
    //Execution flags
    int n_flag = 0;
    int i_flag = 0;
    int g_flag = 0;
    int f_flag = 0;
    int p_flag = 0;
    int o_flag = 0;
    char *generate_path = NULL;
    char *exec_path = NULL;
    char *output_path = NULL;
    int c;
    opterr = 0;

    while ((c = getopt (argc, argv, "i:n:g:f:o:p")) != -1)
	switch (c) {
	case 'n':
	    nb_particle = atoi(optarg);
	    n_flag = 1;
	    break;
	case 'i':
	    nb_iter = atoi(optarg);
	    i_flag = 1;
	    break;
	case 'g':
	    generate_path = optarg;
	    g_flag = 1;
	    break;
	case 'f':
	    exec_path = optarg;
	    f_flag = 1;
	    break;
	case 'o':
	    output_path = optarg;
	    o_flag = 1;
	    break;
	case 'p':
	    p_flag = 1;
	    break;
	case '?':
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	    usage();
	    return EXIT_FAILURE;
	default:
	    usage();
	    return EXIT_FAILURE;
	    break;
	}

    /*-g option: generate a random non-static universe with nb_particles*/
    if(g_flag){
	if(n_flag == 0)
	    printf("Number of particles not set, using default value.\n");
	return main_generate(nb_particle, generate_path);
    }

    /*-f option: sequential execution of the given .univ file*/
    if(f_flag){
	if(i_flag == 0)
	    printf("Number of iterations not set, using default value.\n");
	return main_seq(nb_particle, exec_path, nb_iter, output_path, p_flag, o_flag);
    }

    usage();
    return EXIT_SUCCESS;
}
