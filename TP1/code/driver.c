#include <stdlib.h>
#include <math.h>
#include "dgemm.h"
#include "cblas.h"
#include "util.h"

#define NB_TESTS 4
#define ERROR_BOUND 1E-6

/* int check_matrix(double *m1, double *m2){ */
/*   int res = 1; */
/*   for(int i=0;i < n*m; i++) */
/*     if(m2[i] != m1[i]) */
/*       return 0; */
/*   return res; */
/* } */

int test_cblas_dgemm(){
  int m_a, n_a, m_b, n_b;
  m_a=n_b=3;
  n_a=m_b=2;
  double a[] = {2.55555555555555,8,12,4,1.66666666666666,3};
  double b[] = {3,1,1,3.22222222222222,7,4};
  double expected_res[] = {11.66666666666665,25.66666666666666,39,15.44444444444443,13.37037037037035,21.66666666666666,33.88888888888885,62.66666666666664,96};
  double *res = alloc_matrix(m_a,n_b);
  init_matrix(m_a, n_b, res, 1);

  //alpha beta not used
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m_a, n_b , n_a, 0.0, a, m_a, b, m_b, 0.0, res, m_a);
  int test_validate;
  for(int i=0; i<m_a*n_b; i++){
    test_validate = (fabs(res[i]-expected_res[i])/fabs(expected_res[i]) < ERROR_BOUND);
    if(!test_validate){
      free(res);
      return 0;
    }
  }
  
  free(res);
  return 1;
}

int test_cblas_dgemm_trans(){
  int m_a, n_a, m_b, n_b;
  m_a=n_b=n_a=m_b=3;
  
  double a[] = {2.55555555555555,8,12,4,1.66666666666666,3,2,2,1};
  double b[] = {3,1,1,3.22222222222222,7,4,1,8,2};
  double expected_res[] = {27.66666666666665,16.66666666666666,9,112.23456789012345,36.55555555555555,24.44444444444444,90.55555555555555,23.33333333333328,20};
  double *res = alloc_matrix(m_a,n_b);
  init_matrix(m_a, n_b, res, 1);

  cblas_dgemm(CblasRowMajor, CblasTrans, CblasNoTrans, m_a, n_b , n_a,0.0, a, m_a, b, m_b,0.0, res, m_a);
  int test_validate;
  for(int i=0; i<m_a*n_b; i++){
    test_validate = (fabs(res[i]-expected_res[i])/fabs(expected_res[i]) < ERROR_BOUND);
    if(!test_validate){
      free(res);
      return 0;
    }
  }
    
  free(res);
  return 1;
}


int test_cblas_dgemm_scalaire(){
  int m_a, n_a, m_b, n_b;
  m_a=n_b=3;
  n_a=m_b=2;
  double a[] = {2.55555555555555,8,12,4,1.66666666666666,3};
  double b[] = {3,1,1,3.22222222222222,7,4};
  double expected_res[] = {11.66666666666665,25.66666666666666,39,15.44444444444443,13.37037037037035,21.66666666666666,33.88888888888885,62.66666666666664,96};
  double *res = alloc_matrix(m_a,n_b);
  init_matrix(m_a, n_b, res, 1);

  //alpha beta not used
  cblas_dgemm_scalaire(CblasRowMajor, CblasNoTrans, CblasNoTrans, m_a, n_b , n_a, 0.0, a, m_a, b, m_b, 0.0, res, m_a);
  int test_validate;
  for(int i=0; i<m_a*n_b; i++){
    test_validate = (fabs(res[i]-expected_res[i])/fabs(expected_res[i]) < ERROR_BOUND);
    if(!test_validate){
      free(res);
      return 0;
    }
  }
  
  free(res);
  return 1;

}

int test_cblas_ddot(){
  int vector_size = 4;
  double v1[] = {1,2.55555555555555,4.88888888888888,1.22222222222222};
  double v2[] = {1,1.55555555555555,2,3};
  double expected_res = 18.41975308640971;
  double res = cblas_ddot(vector_size,v1,1,v2,1);
  
  return (fabs(res-expected_res)/fabs(expected_res) < ERROR_BOUND);
}

/***************************************************************************/

typedef struct{
  int(*fun)(void);
  char *msg;
}test_function_t;


test_function_t init_test(int (*fun)(void),char *msg){
  test_function_t tf;
  tf.fun = fun;
  tf.msg = msg;
  return tf;
}

int main(){
  test_function_t tests[NB_TESTS] = {init_test(test_cblas_ddot,"DDOT TEST") , 
				     init_test(test_cblas_dgemm_scalaire, "DGEMM SCALAIRE"),
				     init_test(test_cblas_dgemm, "DGEMM TEST A NO TRANS"),
				     init_test(test_cblas_dgemm_trans, "DGEMM TEST A TRANS")};
				     
  int ret;
  int passed = 0;
  for(int i=0; i<NB_TESTS; i++){
    ret = tests[i].fun();
    passed+=ret;
    printf("%-25s%6s\n", tests[i].msg, (!ret)?"\033[31;1mFAILED\033[0m":"\033[32;1mPASSED\033[0m");
  }
  printf("\n%d out of %d tests passed.\033[0m\n",passed,NB_TESTS);
}
