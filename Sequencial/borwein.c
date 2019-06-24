/* Trabalho de Sistemas Operacionais I

Leonardo Alves Paiva 10276911
Rafael Pastre 9783675

Cod. sequncial para calcular pi - Metodo de Borwein

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

#define N 100000		//10E5

mpf_t* borwein();

int main () {
	printf("Met. de Borwein:  ");
	mpf_out_str(stdout, 10, 7, *borwein(N));
	printf("\n");
	return 0;
}

//Metodo de Borwein
mpf_t* borwein(int n) {
	int i;
	mpf_t a, b, p;
	mpf_t a_tmp, b_tmp, p_tmp , sqrt_a, aux;
	mpf_t* pi;
	
	//Define a precisao
	pi = malloc(sizeof(mpf_t));
	mpf_set_default_prec(pow(10,5));

	//Valores iniciais
	mpf_init_set_d(a, sqrt(2.0));
	mpf_init_set_d(b, 0.0);
	mpf_init_set_d(p, 2.0 + sqrt(2.0));
	mpf_init_set_d(*pi, 0.0);

	//Inicializacao de variaveis sem valor inicial
	mpf_init(a_tmp);
	mpf_init(b_tmp);
	mpf_init(p_tmp);
	mpf_init(sqrt_a);
	mpf_init(aux);
	
	//itera n vezes para calcular pi
	for (i = 0; i < n; i++) {
		mpf_sqrt(sqrt_a, a);

		mpf_ui_div(a_tmp, 1.0, sqrt_a);	//a_tmp = ( sqrt(a) + (1 / sqrt(a)) ) / 2.0;
		mpf_add(a_tmp, a_tmp, sqrt_a);
		mpf_div_ui(a_tmp, a_tmp, 2);

		mpf_add(aux, a, b);		//b_tmp = ( (1 + b) * sqrt(a) ) / (a + b);
		mpf_add_ui(b_tmp, b, 1.0);
		mpf_mul(b_tmp, b_tmp, sqrt_a);
		mpf_div(b_tmp, b_tmp, aux);

		mpf_add_ui(aux, b_tmp, 1.0);	//p_tmp = ( (1 + a_tmp) * p * b_tmp) / (1 + b_tmp);
		mpf_add_ui(p_tmp, a_tmp, 1.0);
		mpf_mul(p_tmp, p_tmp, p);
		mpf_mul(p_tmp, p_tmp, b_tmp);
		mpf_div(p_tmp, p_tmp, aux);
	
		//Atualiza variaveis
		mpf_set(a, a_tmp);
		mpf_set(b, b_tmp);
		mpf_set(p, p_tmp);
		
	}
	
	mpf_set(*pi, p);
	
	//Retorna valor de pi
	return pi;
}

