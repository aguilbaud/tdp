#include <pthread.h>
#include "queue.h"

typedef struct args_s{
  int K;
  double *C;
  const double *A;
  const double *B;
  int lda;
  int ldb;
  int Row;
  int Col;
}args_t;


struct job_s;
TAILQ_HEAD(job_list_s, job_s);
typedef struct job_s {
  TAILQ_ENTRY(job_s) entry;
  args_t args;
}job_t;

static struct job_list_s job_list;



typedef struct args_task_s {
  pthread_mutex_t mut;
  pthread_cond_t cond;
  int nb_thread;
}args_task_t;


typedef struct task_s {
  TAILQ_ENTRY(job_s) entry;
  args_t args;
}task_t;

void *cell_calc(void * arg)
{
  args_task_t *args = arg;

  int nb_asleep = 0;
  int done;
  
  while (1) {
    task_t * task = NULL;
    pthread_mutex_lock(&(args->mut));

    if(nb_asleep == args->nb_thread-1 && TAILQ_EMPTY(&job_list)) {
      done = 1;
      pthread_cond_broadcast(&(args->cond));
    } 
    
    while (TAILQ_EMPTY(&job_list) || !done) {
      nb_asleep++;
      pthread_cond_wait(&(args->cond), &(args->mut));
      nb_asleep--;
    }

    if (!done) {
      task = get_next_job();
    }

    pthread_mutex_unlock(&(args->mut));

    if (task == NULL)
      break;

    do_job(task, &(args->cond));
  }

  return NULL;


  for(int k=0; k<arg->K; k++)
    *(arg->C) += arg->A[k*arg->lda + arg->Row] * arg->B[arg->Col*arg->ldb + k];
}

void cblas_dgemm_scalaire_threads(const int M, const int N, const int K,
				  const double *A, const int lda, 
				  const double *B, const int ldb,
				  double *C, const int ldc)
{
  pthread_t t[NB_THREADS];
  arg_task_t args_task = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NB_THREAD };
  
  for(int p=0;p<NB_THREADS; p++)
    pthread_create(&t[p], NULL, cell_calc, &args_task);
  
  
  for(int i=0; i<M; i+=NB_THREADS)
    for(int j=0; j<N; j+=NB_THREADS){
      args_t *arg = malloc(sizeof(args_t));
      arg->K = K;
      arg->C = C;
      arg->lda = lda;
      arg->ldb = ldb;
      arg->Row = i;
      arg->Col = j;
    }
}

 
/* for(int p=0;p<NB_THREADS; p++) */
/*   pthread_join(p[p], NULL); */
    
