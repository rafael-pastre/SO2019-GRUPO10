/* Trabalho de Sistemas Operacionais I

Leonardo Alves Paiva 10276911
Rafael Pastre 9783675

Cod. sequncial para calcular pi - Metodo de Monte Carlo
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 1000000000		//10E9

double monteCarlo();

int main () {
	printf("Met. de Monte Carlo:  %.6f\n", monteCarlo(N));
	return 0;
}

double monteCarlo(unsigned long int n) {
	int i, count = 0;
	double x, y;
	
	srand(time(NULL));
	
	for (i = 0; i < n; i++) {
		x = (double) rand() / RAND_MAX;
		y = (double) rand() / RAND_MAX;
		
		if ((x * x + y * y) < 1.0) count++;
	}
	
	//Retorna valor de pi
	return (4.0 * count) / n;
}
