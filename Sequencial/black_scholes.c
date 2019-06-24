/* Black Scholes - Precificar opcoes europeias de acoes */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define CLOCKS 	1000
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

double media(double*, int);
double variancia(double, double*, int);

int main() {
	FILE* fp;
	int i, j, M, n;
	double S, E, r, vol, T, confmin, confmax, confwidth, stddev, mean, t, mean_par, var;
	double start, stop, tempo_exec;
	
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

	double trials[M];
	srand(time(NULL));	//Semente
	
	start = (double) clock () / CLOCKS;		//Inicia contagem de tempo
	//Calculo
	for(j = 0; j < n; j++){
		for (i = 0; i < M; i++) {
			t = S * exp((r - vol*vol/2) * T + vol*sqrt(T)*rand()/RAND_MAX);
			trials[i] = exp(-r*T) * MAX(t - E, 0);
		}

		mean_par = media(trials, M);
		mean += mean_par;
		var += variancia(mean_par, trials, M);
	}

	mean /= (double)n;
	var /= (double)n;
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
