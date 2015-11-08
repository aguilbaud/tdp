#include <pthread.h>
#include "dgemm.h"
#include "cblas.h"

#define BLOCK_SIZE 7
#define NB_THREADS 4
#define MIN(A,B) (((A)<(B)) ? (A) : (B))



void cblas_dgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
		 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
		 const int K, const double alpha, const double *A,
		 const int lda, const double *B, const int ldb,
		 const double beta, double *C, const int ldc)
{
  
  
  for(int i=0; i<M; i+=BLOCK_SIZE)
    for(int j=0; j<N; j+=BLOCK_SIZE)
      //IF C not = 0, clear blockC(i,j)
      for(int k=0; k<K; k+=BLOCK_SIZE){
	cblas_dgemm_scalaire(Order, TransA, TransB,
			     MIN(M-i, BLOCK_SIZE), MIN(N-j,BLOCK_SIZE), MIN(K-k,BLOCK_SIZE) , 
			     alpha, TransA==CblasNoTrans?&A[k*lda + i]:&A[i*lda + k] , M, 
			     TransB==CblasNoTrans?&B[j*ldb + k]:&B[k*ldb + j], K, 
			     beta, &C[j*ldc + i], M);
      }
  
}



void cblas_dgemm_scalaire(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
			  const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
			  const int K, const double alpha, const double *A,
			  const int lda, const double *B, const int ldb,
			  const double beta, double *C, const int ldc)
{
    int line_A, col_A, line_B, col_B;

    for(int i=0; i<M; i++){//lignes de A
	for(int j=0; j<N; j++){//colonnes de B
	    for(int k=0; k<K; k++){//lignes de B
		if(TransA==CblasNoTrans){
		    line_A = i;
		    col_A = k;
		}
		else{
		    line_A = k;
		    col_A = i;
		}
		if(TransB==CblasNoTrans){
		    line_B = k;
		    col_B = j;
		}
		else{
		    line_B = j;
		    col_B = k;
		}
		C[j*ldc + i] += A[col_A*lda + line_A] * B[col_B*ldb + line_B] *alpha;
	    }
	}
    }
}
 




/***********DGEMM SCLAIRE TRANSPOSEE (Uniquement pour générer les courbes****************/
void cblas_dgemm_scalaire_ijk(const int M, const int N, const int K,
			      const double *A, const int lda,
				  const double *B, const int ldb,
				  double *C, const int ldc)
    {
	for(int i=0; i<M; i++){//lignes de A
	    for(int j=0; j<N; j++){//colonnes de B
		for(int k=0; k<K; k++){//lignes de B
		    C[j*ldc + i] += A[i*lda + k] * B[j*ldb + k];
		}
	    }
	}
    }

    //C must be filled with 0
    void cblas_dgemm_scalaire_kij(const int M, const int N, const int K,
				  const double *A, const int lda, 
				  const double *B, const int ldb,
				  double *C, const int ldc)
    {
	for(int k=0; k<K; k++){//lignes de B
	    for(int i=0; i<M; i++){//lignes de A
		for(int j=0; j<N; j++){//colonnes de B
		    C[j*ldc + i] += A[i*lda + k] * B[j*ldb + k];
		}
	    }
	}
    }

    void cblas_dgemm_scalaire_jik(const int M, const int N, const int K,
				  const double *A, const int lda, 
				  const double *B, const int ldb,
				  double *C, const int ldc)
    {
	for(int j=0; j<N; j++){//colonnes de B
	    for(int i=0; i<M; i++){//lignes de A
		for(int k=0; k<K; k++){//lignes de B
		    C[j*ldc + i] += A[i*lda + k] * B[j*ldb + k];
		}
      
	    }
	}
    }
