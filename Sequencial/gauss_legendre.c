/* Trabalho de Sistemas Operacionais I

Leonardo Alves Paiva 10276911
Rafael Pastre 9783675

Cod. sequncial para calcular pi - Metodo de Gauss-Legendre

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

#define N 100000		//10E5

mpf_t* gauss_legendre();

int main () {
	printf("Met. de Gauss-Legendre:  ");
	mpf_out_str(stdout, 10, 7, *gauss_legendre(N));
	printf("\n");
	return 0;
}

//Metodo de Gauss-Legendre
mpf_t* gauss_legendre(int n) {
	int i;
	mpf_t* pi;
	mpf_t a, b, t, p;
	mpf_t a_tmp, b_tmp, t_tmp, p_tmp, aux;
	
	//Define a precisao
	pi = malloc(sizeof(mpf_t));
	mpf_set_default_prec(pow(10,5));

	//Valores iniciais
	mpf_init_set_d(a, 1.0);
	mpf_init_set_d(b, (1.0 / sqrt(2.0)));
	mpf_init_set_d(t, 0.25);
	mpf_init_set_d(p, 1.0);
	mpf_init_set_d(*pi, 0.0);

	//Inicializacao de variaveis sem valor inicial
	mpf_init(a_tmp);
	mpf_init(b_tmp);
	mpf_init(t_tmp);
	mpf_init(p_tmp);
	mpf_init(aux);
	
	//itera n vezes para calcular pi
	for (i = 0; i < n; i++) {
		mpf_add(aux, a, b);			//a_tmp = (a + b) / 2.0;
		mpf_div_ui(a_tmp, aux, 2);

		mpf_mul(aux, a, b);			//b_tmp = sqrt(a * b);
		mpf_sqrt(b_tmp, aux);

		mpf_sub(aux, a, a_tmp);		//t_tmp = t - p * pow(a - a_tmp, 2);
		mpf_pow_ui(aux,aux,2);
		mpf_mul(aux, p, aux);
		mpf_sub(t_tmp, t, aux);

		mpf_mul_ui(p_tmp, p, 2);	//p_tmp = 2 * p;
		
		//Atualiza variaveis
		mpf_set(a, a_tmp);
		mpf_set(b, b_tmp);
		mpf_set(t, t_tmp);
		mpf_set(p, p_tmp);
	}
	
	mpf_add(aux, a, b);			//pi = pow(a + b, 2) / (4 * t);
	mpf_pow_ui(aux,aux,2);
	mpf_div_ui(aux,aux,4);
	mpf_div(*pi,aux,t);
	
	//Retorna valor de pi
	return pi;
}
