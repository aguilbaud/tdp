#include <stdlib.h>
#include "util.h"
#include "perf.h"
#include "cblas.h"
#include "dgetf2.h"

#define MAX_MATRIX_SIZE 1000

void timing_cblas_dgetf2_nopiv(){
    FILE * f = fopen("curves/data/dgetf2_nopiv_mflops.dat","w+");
    int N = 100;

    while(N < MAX_MATRIX_SIZE){
	double *A = alloc_matrix(N, N);
	init_matrix(N, N, A, 0);
    
	perf_t p1, p2;
	perf(&p1);
	dgetf_2_nopiv(N, N, A, N);
	perf(&p2);
    
	perf_diff(&p1,&p2);
	double mflops = perf_mflops(&p2,N*N*(N-1));
	fprintf(f,"%d %f\n",N ,mflops);
    
	N*=1.25;
	free(A);
    }
    fclose(f);
}


typedef struct{
    void(*fun)(void);
	char *msg;
}timing_function_t;


timing_function_t init_test(void (*fun)(void),char *msg){
    timing_function_t tf;
    tf.fun = fun;
    tf.msg = msg;
    return tf;
}

int main(){

    int NB_TESTS = 1;
    timing_function_t tests[] = {init_test(timing_cblas_dgetf2_nopiv,
					   "DGETF2 TIMING")}; 


    for(int i=0; i<NB_TESTS; i++){
	printf("%s\n",tests[i].msg);
	tests[i].fun();
    }
    
    return 0;
}
