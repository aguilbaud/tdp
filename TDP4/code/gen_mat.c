#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 100
#define MIN -100

void usage(){
    printf("./gen_mat <N> <file>\n");
}

int main(int argc, char *argv[])
{
    if(argc != 3){
	usage();
	return 0;
    }
    int n = atoi(argv[1]);
    FILE *fp = fopen(argv[2], "w+");
    srand(time(NULL));

    fprintf(fp, "%d\n", n);
    for (int i=0; i < n; i++) {
	for (int j=0; j < n; j++) {
	    fprintf(fp, "%f ", ( rand()/(double)RAND_MAX ) * (MAX-MIN) + MIN);
	}
	fprintf(fp,"\n");
    }

    
    return 0;
}

