/* Trabalho de Sistemas Operacionais I

Leonardo Alves Paiva 10276911
Rafael Pastre 9783675

Cod. multi-thread para calcular pi - Gauss-Legendre

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <gmp.h>

#define N 		100000		//Numero de iteracoes

//Argumento para Gauss-Legendre paralelo
typedef struct {
	mpf_t* pa;
	mpf_t* pb;
	mpf_t* pp;
	mpf_t* pb_tmp;
	mpf_t* pp_tmp;
	int* pflag;
	pthread_barrier_t* pbarrier;
}arguments_gl;

mpf_t* gauss_legendre();
void* gauss_legendre_thread_b();
void* gauss_legendre_thread_p();

int main () {
	printf("Met. de Gauss-Legendre:  ");
	mpf_out_str(stdout, 10, 7, *gauss_legendre(N));
	printf("\n");
	return 0;
}

//Metodo de Gauss-Legendre
mpf_t* gauss_legendre(int n) {
	int i, flag;
	mpf_t* pi;
	mpf_t a, b, t, p;
	mpf_t a_tmp, b_tmp, t_tmp, p_tmp, aux;
	pthread_t thread_b;
	pthread_t thread_p;
	arguments_gl args;
	pthread_barrier_t barrier;
	
	//Define a precisao
	pi = malloc(sizeof(mpf_t));
	mpf_set_default_prec(pow(10,5));

	//Inicializacao
	flag = 1;
	pthread_barrier_init(&barrier, NULL, 3);

	mpf_init_set_d(a, 1.0);
	mpf_init_set_d(b, (1.0 / sqrt(2.0)));
	mpf_init_set_d(t, 0.25);
	mpf_init_set_d(p, 1.0);
	mpf_init_set_d(*pi, 0.0);

	mpf_init(a_tmp);
	mpf_init(b_tmp);
	mpf_init(t_tmp);
	mpf_init(p_tmp);
	mpf_init(aux);

	//Argumentos passados para as threads auxiliares
	args.pa = &a;
	args.pb = &b;
	args.pp = &p;
	args.pb_tmp = &b_tmp;
	args.pp_tmp = &p_tmp;
	args.pflag = &flag;
	args.pbarrier = &barrier;

	//Cria threads auxiliares
	if(pthread_create(&thread_b, NULL, gauss_legendre_thread_b, &args)) {
		printf("Error creating Gauss-Legendre thread\n");
		return 0;
	}
	if(pthread_create(&thread_p, NULL, gauss_legendre_thread_p, &args)) {
		printf("Error creating Gauss-Legendre thread\n");
		return 0;
	}

	//itera n vezes ou ate convergir para calcular pi
	for (i = 0; i < n; i++) {
		mpf_add(aux, a, b);			//a_tmp = (a + b) / 2.0;
		mpf_div_ui(a_tmp, aux, 2);

		mpf_sub(aux, a, a_tmp);			//t_tmp = t - p * pow(a - a_tmp, 2);
		mpf_pow_ui(aux,aux,2);
		mpf_mul(aux, p, aux);
		mpf_sub(t_tmp, t, aux);

		//Espera calculos nas outras threads
		pthread_barrier_wait(&barrier);

		//Atualiza variaveis
		mpf_set(a, a_tmp);
		mpf_set(t, t_tmp);

		//Espera atualizacao de variaveis nas outras threads
		pthread_barrier_wait(&barrier);
	}
	//Finaliza threads auxiliares
	flag = 0;

	//Espera o fim da execucao das threads
	if(pthread_join(thread_b, NULL)) {
		printf("Error joining thread %d\n", i);
		return 0;
	}
	if(pthread_join(thread_p, NULL)) {
		printf("Error joining thread %d\n", i);
		return 0;
	}

	//Calcula valor final de pi
	mpf_add(aux, a, b);			//pi = pow(a + b, 2) / (4 * t);
	mpf_pow_ui(aux,aux,2);
	mpf_div_ui(aux,aux,4);
	mpf_div(*pi,aux,t);

	//Retorna valor de pi
	return pi;
}

//Thread para calculo de 'b' no metodo de Gauss-Legendre
void* gauss_legendre_thread_b(void* thread_arguments){
	arguments_gl* args = (arguments_gl*)thread_arguments;
	mpf_t* pa = args->pa;
	mpf_t* pb = args->pb;
	mpf_t* pb_tmp = args->pb_tmp;
	int* pflag = args->pflag;
	pthread_barrier_t* pbarrier = args->pbarrier;

	while(*pflag == 1){
		//Realizacao dos calculos
		mpf_mul(*pb_tmp, *pa, *pb);			//*pb_tmp = sqrt((*pa) * (*pb));
		mpf_sqrt(*pb_tmp, *pb_tmp);

		//Espera calculos nas outras threads
		pthread_barrier_wait(pbarrier);

		//Atualiza variaveis
		mpf_set(*pb, *pb_tmp);

		//Espera atualizacao de variaveis nas outras threads
		pthread_barrier_wait(pbarrier);
	}

	return NULL;
}

//Thread para calculo de 'p' no metodo de Gauss-Legendre
void* gauss_legendre_thread_p(void* thread_arguments){
	arguments_gl* args = (arguments_gl*)thread_arguments;
	mpf_t* pp = args->pp;
	mpf_t* pp_tmp = args->pp_tmp;
	int* pflag = args->pflag;
	pthread_barrier_t* pbarrier = args->pbarrier;

	while(*pflag == 1){
		//Realizacao dos calculos
		mpf_mul_ui(*pp_tmp, *pp, 2);		//*pp_tmp = 2 * (*pp);

		//Espera calculos da thread principal
		pthread_barrier_wait(pbarrier);

		//Atualiza variaveis
		mpf_set(*pp, *pp_tmp);

		//Espera atualizacao de variaveis na thread principal
		pthread_barrier_wait(pbarrier);
	}

	return NULL;
}
