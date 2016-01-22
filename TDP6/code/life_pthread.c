//code barrier()
pthread_mutex_lock(&mtx);
id = barrier_id;
barrier++;
if(barrier == nb_threads){
  barrier = 0;
  barrier_id ++;
  pthread_cond_broadcast(&cond);
 }


while(id==id_barrier){
  pthread_cond_wait(&cond,&mtx);
 }
pthread_mutex_unlock(&mtx);


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

int BS;
int nb_threads;
pthread_cond_t* cond;
pthread_mutex_t* muts;
pthread_barrier_t barrier;
sem_t* sems;

#define cell( _i_, _j_ ) board[ ldboard * (_j_) + (_i_) ]
#define ngb( _i_, _j_ )  nbngb[ ldnbngb * ((_j_) - 1) + ((_i_) - 1 ) ]

double mytimer(void)
{
    struct timeval tp;
    gettimeofday( &tp, NULL );
    return tp.tv_sec + 1e-6 * tp.tv_usec;
}

void output_board(int N, int *board, int ldboard, int loop)
{
    int i,j;
    printf("loop %d\n", loop);
    for (i=0; i<N; i++) {
	for (j=0; j<N; j++) {
	    if ( cell( i, j ) == 1)
		printf("X");
	    else
		printf(".");
	}
	printf("\n");
    }
}

/**
 * This function generates the iniatl board with one row and one
 * column of living cells in the middle of the board
 */
int generate_initial_board(int N, int *board, int ldboard)
{
    int i, j, num_alive = 0;

    for (i = 0; i < N; i++) {
	for (j = 0; j < N; j++) {
	    if (i == N/2 || j == N/2) {
		cell(i, j) = 1;
		num_alive ++;
	    }
	    else {
		cell(i, j) = 0;
	    }
	}
    }

    return num_alive;
}

typedef struct param_s{
    int maxloop;
    int *board;
    int *nbngb;
    int ldboard;
    int ldnbngb;
    int id;
    int num_alive_local;
  
}param_t;

void * f_threads( void * param){

    param_t *p = (param_t*) param;
    int *nbngb = p->nbngb;
    int ldnbngb = p->ldnbngb;
    int *board = p->board;
    int ldboard = p->ldboard;
    int maxloop = p->maxloop;
    int num_alive = p->num_alive_local;

    int beg = p->id*BS/nb_threads + 1;
    int end = (p->id+1)*BS/nb_threads;
    int nbgs[2] = {(p->id-1 + nb_threads)%nb_threads, (p->id+1)%nb_threads};
    //printf("coucou th %d %d %d %d\n", p->id, maxloop, ldboard, ldnbngb);
    //printf("neigh %d %d\n", nbgs[0], nbgs[1]);
    
    for (int loop = 1; loop <= maxloop; loop++) {

	//Recopie des cellules fantômes
	if(p->id == 0){
	    cell(   0, 0   ) = cell(BS, BS);
	    cell(BS+1, 0   ) = cell( 1, BS);
	    for (int i = 1; i <= BS; i++) 
		cell(i, 0) = cell(i, BS);
	}

	if(p->id == nb_threads - 1){
	    cell(   0, BS+1) = cell(BS,  1);	
	    cell(BS+1, BS+1) = cell( 1,  1);
	    for (int i = 1; i <= BS; i++) 
		cell(i, BS+1) = cell(i,  1);
	}
		
	for (int i = 1; i <= BS; i++) {	    
	    cell(   0,    i) = cell(BS,  i);
	    cell(BS+1,    i) = cell( 1,  i);
	}

	//Calcul des voisins sur ses bords
	//printf("Th %d calcul voisins bords cols: %d %d\n", p->id, beg, end);
	int j = beg;
	for(int k = 0; k<2; k++){
	    for (int i = 1; i <= BS; i++) {
		ngb( i, j ) =
		    cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
		    cell( i-1, j   ) +                  cell( i+1, j   ) +
		    cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
		//printf("cell(%d,%d) = %d\n",i, j,ngb(i,j));
	    }
	    j = end;
	}
	
	int sem_val;
	if(p->id == 1){
	    sem_getvalue(&sems[0], &sem_val);
	    printf("sem 0, %d \n", sem_val);
	}

	sem_post(&sems[nbgs[0]]);
	sem_post(&sems[nbgs[1]]);

	if(p->id == 1){
	    sem_getvalue(&sems[0], &sem_val);
	    printf("sem 0, %d \n", sem_val);
	}

	//On informe les voisins qu'on a fini de lire leurs cellules

	//On calcule les voisins des cellules internes
	//printf("Th %d calcul voisins internes cols: [%d,%d]\n", p->id, beg+1, end-1);
	for (int j = beg+1; j <= end-1; j++) {
	    for (int i = 1; i <= BS; i++) {
		ngb( i, j ) =
		    cell( i-1, j-1 ) + cell( i, j-1 ) + cell( i+1, j-1 ) +
		    cell( i-1, j   ) +                  cell( i+1, j   ) +
		    cell( i-1, j+1 ) + cell( i, j+1 ) + cell( i+1, j+1 );
	    }
	}

	

	//Mise à jour des cellules internes
	num_alive = 0;
	for (int j = beg+1; j <= end-1; j++) {
	    for (int i = 1; i <= BS; i++) {
		if ( (ngb( i, j ) < 2) ||
		     (ngb( i, j ) > 3) ) {
		    cell(i, j) = 0;
		}
		else {
		    if ((ngb( i, j )) == 3)
			cell(i, j) = 1;
		}
		if (cell(i, j) == 1) {
		    num_alive ++;
		}
	    }
	}

	//Attendre que les voisins aient finis de lire nos cellules
	if(p->id == 1){
	    sem_getvalue(&sems[0], &sem_val);
	    printf("sem 0, %d \n", sem_val);
	}
	sem_wait(&sems[p->id]);
	if(p->id == 1){
	    sem_getvalue(&sems[0], &sem_val);
	    printf("sem 0, %d \n", sem_val);
	}
	//Mise à jour des bords	
	j = beg;
	for(int k = 0; k<2; k++){
	    for (int i = 1; i <= BS; i++) {
		if ( (ngb( i, j ) < 2) ||
		     (ngb( i, j ) > 3) ) {
		    cell(i, j) = 0;
		}
		else {
		    if ((ngb( i, j )) == 3)
			cell(i, j) = 1;
		}
		if (cell(i, j) == 1) {
		    num_alive ++;
		}
	    }
	    j=end;
	}
	
	//Barriere de synchro
	pthread_barrier_wait(&barrier);
    }

    p->num_alive_local = num_alive;
    return NULL;
}

int main(int argc, char* argv[])
{
    int i, j, loop, num_alive, maxloop;
    int ldboard, ldnbngb;
    double t1, t2;
    double temps;

    int *board;
    int *nbngb;
  
    if (argc < 4) {
	printf("Usage: %s nb_iterations size nb_threads\n", argv[0]);
	return EXIT_SUCCESS;
    } else {
	maxloop = atoi(argv[1]);
	BS = atoi(argv[2]);
	nb_threads = atoi(argv[3]);
	//printf("Running sequential version, grid of size %d, %d iterations\n", BS, maxloop);
    }
    pthread_t t[nb_threads];
    param_t p[nb_threads];

    num_alive = 0;
    
    //Création des threads + mutex
    cond = malloc(nb_threads*sizeof(pthread_cond_t));
    muts = malloc(nb_threads*sizeof(pthread_mutex_t));
    sems = malloc(nb_threads*sizeof(sem_t));
    for (int i = 0; i < nb_threads; ++i){
	pthread_cond_init( &cond[i], NULL);
	pthread_mutex_init( &muts[i], NULL);
	sem_init( &sems[i], 0, 0);
    }
    pthread_barrier_init(&barrier, NULL, nb_threads);
    /* Leading dimension of the board array */
    ldboard = BS + 2;
    /* Leading dimension of the neigbour counters array */
    ldnbngb = BS;

    board = malloc( ldboard * ldboard * sizeof(int) );
    nbngb = malloc( ldnbngb * ldnbngb * sizeof(int) );

    num_alive = generate_initial_board( BS, &(cell(1, 1)), ldboard );
  
    for(int i=0; i< nb_threads; i++){    
	p[i].maxloop = maxloop;
	p[i].board = board;  
	p[i].nbngb = nbngb;
	p[i].ldboard = ldboard;
	p[i].ldnbngb = ldnbngb;
	p[i].id = i;
	p[i].num_alive_local = 0;
    }

	
    printf("Starting number of living cells = %d\n", num_alive);
    t1 = mytimer();
    for (int i = 0; i < nb_threads; ++i){
	pthread_create(&t[i], NULL, f_threads, (void *) &p[i]);
    }

    num_alive = 0;
    int *tmp;
    for (int i = 0; i < nb_threads; ++i){
	pthread_join(t[i], (void *)&tmp);
	num_alive+=p[i].num_alive_local;
    }
   
    /*   /\* Avec les celluls sur les bords (utile pour vérifier les comm MPI) *\/ */
    /*   /\* output_board( BS+2, &(cell(0, 0)), ldboard, loop ); *\/ */

    /*   /\* Avec juste les "vraies" cellules: on commence à l'élément (1,1) *\/ */
    /*   //output_board( BS, &(cell(1, 1)), ldboard, loop); */

    /*   printf("%d cells are alive\n", num_alive); */
    /* } */
    
    t2 = mytimer();
    temps = t2 - t1;
    printf("Final number of living cells = %d\n", num_alive);
    printf("%.2lf\n",(double)temps * 1.e3);
    
    /* for (int i = 0; i < nb_threads; ++i){ */
    /* 	pthread_cond_destroy( &cond[i]); */
    /* 	pthread_mutex_destroy ( &muts[i]); */
    /* } */
    free(cond);
    free(muts);
    free(sems);
    free(board);
    free(nbngb);
    return EXIT_SUCCESS;
}



  
