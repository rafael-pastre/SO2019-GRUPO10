/* Black Scholes - Precificar opcoes de acoes europeisas */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define CLOCKS 	1000
#define THREADS	4
#define MAX_MEM 1000000

#define abrir_arq_leitura(f, arq) if ((f = fopen(arq,"r")) == NULL) { perror("Imp. ler arquivo"); exit(2); }	//Macro para abrir arquivo para leitura
#define abrir_arq_escrita(f, arq) if ((f = fopen(arq,"w")) == NULL) { perror("Imp. criar arquivo"); exit(2); }	//Macro para abrir arquivo para escrita
#define MAX(a, b) ( ( (a) > (b) ) ? (a) : (b) )																//Macro para obter maximo de dois numeros

/* Paramentros do programa
Variaveis de entrada:
S: valor da acao
E: preco de exercicio da opcao
r: taxa de juros livre de risco (SELIC)
vol: volatilidade da acao
T : tempo de validade da opcao
M : numero de iteracoes

Saida do programa:
Variaveis de entrada
Tempo de execucao
Intervalo de confianca (margem inferior e margem superior)
*/

//Argumentos para Black_Scholes paralelo
typedef struct {
	double S;
	double E;
	double r;
	double vol;
	double T;
	int M;
	int n;
	double pvar;
	double pmean;
}arguments_bs;

typedef int random_number;

double media(double*, int);
double variancia(double, double*, int);
void* black_scholes_thread(void* thread_arguments);
random_number randomize(random_number* r);

int main() {
	FILE* fp;
	int i, j, M, n;
	double S, E, r, vol, T, confmin, confmax, confwidth, stddev, mean, mean_par, t, var;
	double start, stop, tempo_exec;
	pthread_t threads[THREADS-1];
	arguments_bs args[THREADS-1];
	
	//entrada
	/*
	S = 30.00;	E = 25.00;	r = 0.065;	vol = 0.25;	T = 0.5;
	M = 15;
	*/
	//Leitura do arquivo de entrada
	char s[150];
	abrir_arq_leitura(fp, "entrada_blackscholes.txt");
	fscanf(fp, "%s %lf", s, &S);
	fscanf(fp, "%s %lf", s, &E);
	fscanf(fp, "%s %lf", s, &r);
	fscanf(fp, "%s %lf", s, &vol);
	fscanf(fp, "%s %lf", s, &T);
	fscanf(fp, "%s %d", s, &M);
	fclose(fp);
	
	printf("%lf %lf %lf %lf %lf %d\n", S, E, r, vol, T, M);
	
	if(M > MAX_MEM){
		n = M/MAX_MEM;
		M = MAX_MEM;
	}
	else{
		n = 1;
	}
	
	srand(time(NULL));	//Semente

	double trials[M/THREADS];	
	start = (double) clock () / CLOCKS;		//Inicia contagem de tempo
	//Criacao das threads
	for (i = 0; i < THREADS-1; i++){
		(args[i]).S = S;
		(args[i]).E = E;
		(args[i]).r = r;
		(args[i]).vol = vol;
		(args[i]).T = T;
		(args[i]).M = M;
		(args[i]).n = n;

		if(pthread_create(&(threads[i]), NULL, black_scholes_thread, &(args[i]))) {
			printf("Error creating thread %d\n", i);
			return 0;
		}
	}

	//Calculo
	mean = 0.0;
	var = 0.0;

	for(j = 0; j < n; j++){
		for (i = 0; i < M/THREADS; i++) {
			t = S * exp((r - vol*vol/2) * T + vol*sqrt(T)*rand()/RAND_MAX);
			trials[i] = exp(-r*T) * MAX(t - E, 0);
		}

		mean_par = media(trials, M/THREADS);
		mean += mean_par;
		var += variancia(mean_par, trials, M/THREADS);
	}

	mean /= (double)n;
	var /= (double)n;

	//Aguarda o fim da execucao das threads
	for(i = 0; i < THREADS-1; i++){
		if(pthread_join(threads[i], NULL)) {
			printf("Error joining thread %d\n", i);
			return 0;
		}
		else{
			mean += args[i].pmean;
			var += args[i].pvar;
		}
	}

	mean /= (double)THREADS;
	var /= (double)THREADS;
	stddev = sqrt(var);

	printf("mean %lf\n", mean);
	printf("stddev %lf\n", stddev);
	confwidth = 1.96 * stddev / sqrt((double) M);
	confmin = mean - confwidth;
	confmax = mean + confwidth;

	stop = (double) clock () / CLOCKS;	//Ecerra contagem de tempo
	tempo_exec = (stop - start);
	
	//saida
	printf("%lf %lf\n", confmin, confmax);
	
	//Escrita do arquivo de saída
	abrir_arq_escrita(fp, "saida_blackscholes.txt");
	fprintf(fp, "%s %lf\n", "S ", S);
	fprintf(fp, "%s %lf\n", "E ", E);
	fprintf(fp, "%s %lf\n", "r ", r);
	fprintf(fp, "%s %lf\n", "v ", vol);
	fprintf(fp, "%s %lf\n", "T ", T);
	fprintf(fp, "%s %d\n", "M ", M);
	fprintf(fp, "Runtime: %lf ms\n", tempo_exec);
	fprintf(fp, "Confidence interval: (%lf, %lf)", confmin, confmax);
	fclose(fp);
	
	return 0;	
}

//Calcula media aritmetica
double media(double trials[], int N) {
	double mean = 0.0;
	int i;
	for (i = 0; i < N; i++) {
		mean += trials[i];
	}
	
	mean /= N;
	return mean;
}

//Calcula variancia
double variancia(double mean, double trials[], int N) {
	double stddev = 0.0;
	double temp;
	int i;
	
	for (i = 0; i < N; i++) {
		temp = trials[i] - mean;
		stddev += temp * temp;
	}
	
	stddev /= (N - 1);		//Estimador nao viciado
	
	return stddev;
}

//Thread do metodo de Black Scholes
void* black_scholes_thread(void* thread_arguments){
	arguments_bs* args = (arguments_bs*)thread_arguments;
	double S = args->S;
	double E = args->E;
	double r = args->r;
	double vol = args->vol;
	double T = args->T;
	int M = args->M;
	int n = args->n;
	double pvar = 0.0;
	double pmean = 0.0;
	//double* trials = args->trials;

	random_number rn;
	double* trials;
	double t, mean_par;
	int i, j;

	rn = rand();		//As threads sao inicializadas com valores aleatorios para que os valores da funcao randomize sejam diferentes
	trials = malloc((M/THREADS)*sizeof(double));
	if(trials == NULL){
		printf("erro de memoria. diminua o valor de MAX_MEM\n");
		return NULL;
	}
	
	//Calculo
	for(j = 0; j < n; j++){
		for (i = 0; i < M/THREADS; i++) {
			t = S * exp((r - vol*vol/2) * T + vol*sqrt(T)*((double) randomize(&rn) / RAND_MAX));
			trials[i] = exp(-r*T) * MAX(t - E, 0);
		}

		mean_par = media(trials, M/THREADS);
		pmean += mean_par;
		pvar += variancia(mean_par, trials, M/THREADS);
	}

	pmean /= (double)n;
	pvar /= (double)n;
	
	args->pmean = pmean;
	args->pvar = pvar;
	return NULL;
}

//Gera o proximo numero pseudo-aleatorio e atualiza a variavel r
random_number randomize(random_number* r){
	return *r = (1103515245*(*r)+ 12345) % 0x80000000;
}
