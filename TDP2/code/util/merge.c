#include <stdio.h>
#include <stdlib.h>

#define NB_ITER 100 //nb_iter/rate
#define NB_PARTICLE 200/4 //nb_particle/nb_processus

int main(int argc, char *argv[])
{
    int nb_files = argc - 1;
    FILE ** list_file = malloc(nb_files * sizeof(FILE));
    FILE * output = fopen("merged.dat", "w+");
    if(output == NULL){
	fprintf(stderr, "Error openning file\n");
	return -1;
    }

    for(int i=1; i<argc; i++){
	printf("%s\n", argv[i]);
	list_file[i-1] = fopen(argv[i], "r");
	if(list_file[i-1] == NULL){
	    fprintf(stderr, "Error openning file\n");
	    return -1;
	}
    }

    float m, px, py;
    for(int i = 0; i<NB_ITER; i++){
	for(int j = 0; j < nb_files; j++){
	    for(int k = 0; k < NB_PARTICLE; k++){

		fscanf(list_file[j], "%f %f %f", &m, &px, &py);
		//printf("%f %f %f\n", m, px, py);
		fprintf(output, "%f %f %f\n", m, px, py);
		// fseek(list_file[j], 3, SEEK_CUR);
	    }
	}
	fprintf(output, "\n\n");
    }

    for(int i=0; i<nb_files; i++)
	fclose(list_file[i]);
    fclose(output);

    return 0;
}
