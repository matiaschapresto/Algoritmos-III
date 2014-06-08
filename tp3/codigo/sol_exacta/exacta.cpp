#include <stdio.h>
#include <iostream>
#include "../common/grafo.h"
#include "../common/parser.h"

using namespace std;

typedef struct peso{
	int w1;
	int w2;
	int otra_cosa;
} peso;

void setW1(void *p, float w)
{
	((peso *)p)->w1 = w;
}
void setW2(void *p, float w)
{
	((peso *)p)->w2 = w;
}

int main(int argc, char **argv)
{
	{
		peso p1, *p;
		GrafoAdyacencia<peso> grafo(10);

		p1.w1 = 42;
		p1.w2 = 30;
		grafo.AgregarArista(2,3, &p1);

		cout << grafo.EsAdyacente(2,3) << " " << grafo.EsAdyacente(9,8) << endl;
		p = grafo.Peso(2, 3);
		cout << p->w1 << " " << p->w2 << " " << endl;
	}
	{
		peso p1, *p;
		GrafoLista<peso> grafo(10);

		p1.w1 = 32;
		p1.w2 = 20;
		grafo.AgregarArista(5,3, &p1);

		cout << grafo.EsAdyacente(5,3) << " " << grafo.EsAdyacente(2,3) << endl;
		p = grafo.Peso(5, 3);
		cout << p->w1 << " " << p->w2 << " " << endl;
	}

	GrafoAdyacencia<peso> *grafo;
	peso **pesos;
	int u,v, K, nodos, aristas;
	grafo = new GrafoAdyacencia<peso>(0);
	while(Parsear<peso>(*grafo, stdin, &pesos, setW1, setW2, &u, &v, &K, &nodos, &aristas)){
		cout << "tiene " << grafo->CantidadNodos() << " nodos" << endl;
		int i;
		for(i = 0; i < aristas; i++){
			delete pesos[i];
		}
		free(pesos);
		delete grafo;
		grafo = new GrafoAdyacencia<peso>(0);
	}

	return 0;
}

