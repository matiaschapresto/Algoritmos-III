#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	
	int cantidad_nodos;
	int cantidad_aristas;
	int maximo_random_w1;
	int maximo_random_w2;
	int K;
	int v;
	int w;
	int **aristas;
	int i, j;
	
	struct timeval tv;

	if(argc != 5){
		fprintf(stderr, "%s cantidad_nodos cantidad_aristas maximo_random_w1 maximo_random_w2\nK va a estar aleatoriamente entre 0 y cantidad_aristas * maximo_random_w1\n", argv[0]);
		return -1;
	}
	cantidad_nodos = atoi(argv[1]);
	cantidad_aristas = atoi(argv[2]);
	maximo_random_w1 = atoi(argv[3]);
	maximo_random_w2 = atoi(argv[4]);
	v = random() % cantidad_nodos;
	v++;
	w = random() % cantidad_nodos;
	w++;

	gettimeofday(&tv, NULL);

	srandom(tv.tv_usec);
	K = random() % ((cantidad_aristas * maximo_random_w1) + 1);
	
	printf("%d %d %d %d %d\n", cantidad_nodos, cantidad_aristas, v, w, K);
	aristas = (int **)calloc(cantidad_nodos + 1, sizeof(int *));
	for(i = 0; i <= cantidad_nodos; i++){
		aristas[i] = (int *)calloc(cantidad_nodos + 1, sizeof(int));
	}
	while(cantidad_aristas > 0){
		i = random() % cantidad_nodos;
		i++;
		j = random() % cantidad_nodos;
		j++;
		if(i == j || aristas[i][j] || aristas[j][i])
			continue;
		aristas[i][j] = 1;
		aristas[j][i] = 1;
		printf("%d %d %ld.0 %ld.0\n", i, j, random() % maximo_random_w1, random() % maximo_random_w2);
		cantidad_aristas--;
	}
	printf("0\n");
	return 0;
}

