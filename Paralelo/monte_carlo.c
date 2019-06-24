/* Trabalho de Sistemas Operacionais I

Leonardo Alves Paiva 10276911
Rafael Pastre 9783675

Cod. multi-thread para calcular pi - Monte Carlo

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define N 		1000000000		//Numero de iteracoes
#define THREADS_MC 	4			//Numero de threads no metodo de Monte Carlo
#define RANDOM_MAX 	(0x80000000 - 1)	//Maior numero gerado pela pela funcao randomize

typedef int random_number;

double monteCarlo();
void* monteCarlo_thread(void* pcount_void);
random_number randomize(random_number* r);


int main () {
	printf("Met. de Monte Carlo:  %.6f\n", monteCarlo(N));
	return 0;
}

//Metodo de Monte Carlo
double monteCarlo(unsigned long int n) {
	int i, count = 0;
	int count_thread[THREADS_MC-1];
	pthread_t threads[THREADS_MC-1];

	srand(time(NULL));
	
	
	//Cria as threads
	for(i = 0; i < THREADS_MC-1; i++){
		if(pthread_create(&(threads[i]), NULL, monteCarlo_thread, &(count_thread[i]))) {
			printf("Error creating thread %d\n", i);
			return 0;
		}
	}
	
	//Execucao do algotimo de Monte Carlo na thread principal e nas criadas
	monteCarlo_thread(&count);

	//Aguarda o fim da execucao das threads
	for(i = 0; i < THREADS_MC-1; i++){
		if(pthread_join(threads[i], NULL)) {
			printf("Error joining thread %d\n", i);
			return 1;
		}
		else{
			count += count_thread[i];
		}
	}
	
	//Retorna valor de pi
	return (4.0 * count) / n;
}

//Thread do metodo de Monte Carlo
void* monteCarlo_thread(void* pcount_void){
	int i;
	double x, y;
	int* pcount;
	random_number r;
	
	pcount = (int*)pcount_void;
	r = rand();		//As threads sao inicializadas com valores aleatorios para que os valores da funcao randomize sejam diferentes
	
	for (i = 0; i < (N / THREADS_MC); i++) {	
		x = (double) randomize(&r) / RANDOM_MAX;
		y = (double) randomize(&r) / RANDOM_MAX;
		
		if ((x * x + y * y) < 1.0) (*pcount)++;
	}
	return NULL;
}

//Gera o proximo numero pseudo-aleatorio e atualiza a variavel r
random_number randomize(random_number* r){
	return *r = (1103515245*(*r)+ 12345) % 0x80000000;
}
