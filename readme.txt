Forma de execução dos códigos
Feito por:
Leonardo Alves Paiva 10276911
Rafael Pastre 9783675

Instruções para compilação dos programas:
Códigos sequenciais:
 - Gauss-Legendre:
   gcc gauss_legendre.c -o gauss_legendre -lm -lgmp
 - Borwein:
   gcc borwein -o borwein -lm -lgmp
 - Monte Carlo:
   gcc monte_carlo.c -o monte_carlo -lm
 - Black-Scholes:
   black_scholes.c -o black_scholes -lm
Códigos paralelos:
 - Gauss-Legendre:
   gcc gauss_legendre.c -o gauss_legendre -lm -lgmp -lpthread
 - Borwein:
   gcc borwein -o borwein -lm -lgmp -lpthread
 - Monte Carlo:
   gcc monte_carlo.c -o monte_carlo -lm -pthread
 - Black-Scholes:
   black_scholes.c -o black_scholes -lm -pthread

Instruções para execução dos programas:
 - Gauss-Legendre:
   time ./gauss_legendre
 - Borwein:
   time ./borwein
 - Monte Carlo:
   time ./monte_carlo
 - Black-Scholes:
   time ./black_scholes

Para a excecução do metodo de Black-Scholes, é necessária a presença do arquivo entrada_blackscholes.txt no seguinte formato:
S <valor_de_S>
E <valor_de_E>
r <valor_de_R>
v <valor_de_v>
T <valor_de_T>
M <valor_de_M>
