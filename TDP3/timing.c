#include <stdlib.h>
#include "util.h"
#include "perf.h"
#include "mycblas.h"

#define MAX_MATRIX_SIZE 5000

void timing_cblas_dgetf2_nopiv(){
    FILE * f = fopen("curves/data/dgetf2_nopiv_mflops.dat","w+");
    int N = 10;

    while(N < MAX_MATRIX_SIZE){
	double *A = alloc_matrix(N, N);
	init_matrix(N, N, A, 0);
    
	perf_t p1, p2;
	perf(&p1);
	mycblas_dgetf2_nopiv(N, N, A, N);
	perf(&p2);
	
	
	//long mflops = perf_mflops(&p2,N*N*(N-1));
	unsigned long long mflops = (N*N*(N-1)) / (1000000*(p2.tv_sec - p1.tv_sec) + (p2.tv_usec - p1.tv_usec));
	fprintf(f,"%d %llu\n",N , mflops);
	

	N*=1.5;
	free(A);
    }
    fclose(f);
}

//inter-exter
void timing_cblas_dger(){
    FILE * f = fopen("curves/data/dger_MN_mflops.dat","w+");
    int N = 10;

    while(N < MAX_MATRIX_SIZE){
	double *A = alloc_matrix(N, N);
	double *X = alloc_vector(N);
	double *Y = alloc_vector(N);
	init_vector(N,X);
	init_vector(N,Y);
	init_matrix(N, N, A, 0);
    
	perf_t p1, p2;
	perf(&p1);
	mycblas_dger(CblasColMajor,N, N, 2.0, X, 1, Y, 1, A, N);
	perf(&p2);

	unsigned long long mflops = (N*N*2) / (double)(1000000*(p2.tv_sec - p1.tv_sec) + (p2.tv_usec - p1.tv_usec));
	fprintf(f,"%d %llu\n",N , mflops);
	    
	N*=1.5;
	free(A);
    }
    fclose(f);
}

//sizeblock varie
void timing_cblas_dgetrf(){
    FILE * f = fopen("curves/data/dgetrf_SB_mflops.dat","w+");
    int N = 2000;
    int size_block = 2;
    float f_size_block = 2.0;
    
    double *A = alloc_matrix(N, N);
    init_matrix(N, N, A, 0);
    
    while(size_block < 1000){
	perf_t p1, p2;
	perf(&p1);
	//mycblas_dgetrf(N, N, A, N, NULL, NULL, size_block);
	mycblas_dgetrf(N, N, A, N, NULL, NULL);
	perf(&p2);
	
	
	//unsigned long long mflops = (N*N*(N-1)) / (1000000*(p2.tv_sec - p1.tv_sec) + (p2.tv_usec - p1.tv_usec));
	fprintf(f,"%d %ld\n",size_block , (1000000*(p2.tv_sec - p1.tv_sec) + (p2.tv_usec - p1.tv_usec)));

   
	f_size_block*=1.5;
	size_block = f_size_block+1.0;
    }
    fclose(f);
}

//comparaison dgetf2nopiv dgetrf avec sb trouvé plus tôt
//+dgetrf mpi

/******************************************************/
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

    int NB_TESTS = 3;
    timing_function_t tests[] = {init_test(timing_cblas_dgetf2_nopiv, "DGETF2 TIMING"),
				 init_test(timing_cblas_dger, "DGER TIMING"),
				 init_test(timing_cblas_dgetrf, "DGETRF TIMING")
    }; 



    for(int i=0; i<NB_TESTS; i++){
	printf("%s\n",tests[i].msg);
	tests[i].fun();
    }
    
    return 0;
}
