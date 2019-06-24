/* Trabalho de Sistemas Operacionais I

Leonardo Alves Paiva 10276911
Rafael Pastre 9783675

Cod. multi-thread para calcular pi - Borwein

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <gmp.h>

#define N 		100000		//Numero de iteracoes

//Argumentos para Borwein paralelo
typedef struct {
	mpf_t* pa;
	mpf_t* pb;
	mpf_t* pb_tmp;
	int* pflag;
	pthread_barrier_t* pbarrier;
}arguments_bw;

mpf_t* borwein();
void* borwein_thread(void* thread_arguments);

int main () {
	printf("Met. de Borwein:  ");
	mpf_out_str(stdout, 10, 7, *borwein(N));
	printf("\n");
	return 0;
}

//Metodo de Borwein
mpf_t* borwein(int n) {
	int i, flag;
	mpf_t a, b, p;
	mpf_t a_tmp, b_tmp, p_tmp , sqrt_a, aux;
	mpf_t* pi;
	pthread_t thread;
	arguments_bw args;
	pthread_barrier_t barrier;

	//Define a precisao
	pi = malloc(sizeof(mpf_t));
	mpf_set_default_prec(pow(10,5));

	//Inicializacao
	flag = 1;
	pthread_barrier_init(&barrier, NULL, 2);

	mpf_init_set_d(a, sqrt(2.0));
	mpf_init_set_d(b, 0.0);
	mpf_init_set_d(p, 2.0 + sqrt(2.0));
	mpf_init_set_d(*pi, 0.0);

	mpf_init(a_tmp);
	mpf_init(b_tmp);
	mpf_init(p_tmp);
	mpf_init(sqrt_a);
	mpf_init(aux);

	//Argumentos passados para a thread auxiliar
	args.pa = &a;
	args.pb = &b;
	args.pb_tmp = &b_tmp;
	args.pflag = &flag;
	args.pbarrier = &barrier;

	//Cria thread para auxiliar nos calculos
	if(pthread_create(&thread, NULL, borwein_thread, &args)) {
		printf("Error creating Borwein thread\n");
		return 0;
	}

	//itera n vezes ou ate convergir para calcular pi
	for (i = 0; i < n; i++) {
		mpf_sqrt(sqrt_a, a);
		mpf_ui_div(a_tmp, 1.0, sqrt_a);	//a_tmp = ( sqrt(a) + (1 / sqrt(a)) ) / 2.0;
		mpf_add(a_tmp, a_tmp, sqrt_a);
		mpf_div_ui(a_tmp, a_tmp, 2);
		
		//Espera calculos nas outras threads
		pthread_barrier_wait(&barrier);

		mpf_add_ui(aux, b_tmp, 1.0);	//p_tmp = ( (1 + a_tmp) * p * b_tmp) / (1 + b_tmp);
		mpf_add_ui(p_tmp, a_tmp, 1.0);
		mpf_mul(p_tmp, p_tmp, p);
		mpf_mul(p_tmp, p_tmp, b_tmp);
		mpf_div(p_tmp, p_tmp, aux);
	
		//Atualiza variaveis
		mpf_set(a, a_tmp);
		mpf_set(p, p_tmp);

		//Espera atualizacao de variaveis nas outras threads
		pthread_barrier_wait(&barrier);
	}
	
	flag = 0;
	//Espera o fim da execucao da thread auxiliar
	if(pthread_join(thread, NULL)) {
		printf("Error joining thread %d\n", i);
		return 0;
	}

	//Calcula valor final de pi
	mpf_set(*pi, p);

	//Retorna valor de pi
	return pi;
}

//Thread do metodo de Borwein
void* borwein_thread(void* thread_arguments){
	arguments_bw* args = (arguments_bw*)thread_arguments;
	mpf_t* pa = args->pa;
	mpf_t* pb = args->pb;
	mpf_t* pb_tmp = args->pb_tmp;
	mpf_t aux;

	int* pflag = args->pflag;
	pthread_barrier_t* pbarrier = args->pbarrier;

	mpf_init(aux);

	while (*pflag == 1) {

		//*pb_tmp = ( (1 + *pb) * sqrt(*pa) ) / (*pa + *pb);
		mpf_add_ui(*pb_tmp, *pb, 1.0);
		mpf_sqrt(aux, *pa);
		mpf_mul(*pb_tmp, *pb_tmp, aux);
		mpf_add(aux, *pa, *pb);
		mpf_div(*pb_tmp, *pb_tmp, aux);

		//Espera calculos da thread principal
		pthread_barrier_wait(pbarrier);

		//Atualiza variaveis na thread principal
		mpf_set(*pb, *pb_tmp);

		//Espera atualizacao de variaveis na thread principal
		pthread_barrier_wait(pbarrier);
	}

	return NULL;
}
