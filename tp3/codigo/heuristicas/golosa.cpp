#include "grafo.h"
#include "timing.h"
#define CANT_ITERS_MEDICION 1

void ejecutar_greedy(Grafo &g);

// -------------- Main ---------------------------------
int main(int argc, char **argv){
    list<Grafo> instancias = Grafo::parsear_varias_instancias(FORMATO_1_N_CLOSED);
    uint64_t instance_number = 1;
    for(Grafo &g : instancias){
        #ifdef DEBUG_MESSAGES_ON
            cout << endl << endl << "Aplicando heuristica greedy a la " << instance_number << "-esima instancia de input..." << endl;
        #endif
        ejecutar_greedy(g);
        instance_number++;
    }
    return 0;
}

void ejecutar_greedy(Grafo &g){
    #ifdef DEBUG_MESSAGES_ON
        //g.imprimir_lista_adyacencia(cout);
        //g.imprimir_matriz_adyacencia(cout);
    #endif
    double promedio_medicion = 0;
    double parametro_beta=1;
    //typedef enum tipo_ejecucion_golosa_t {RCL_DETERMINISTICO, RCL_POR_VALOR, RCL_POR_CANTIDAD} tipo_ejecucion_golosa_t;
    Camino camino = g.obtener_camino_vacio();
    MEDIR_TIEMPO_PROMEDIO(
                camino = g.obtener_solucion_golosa(RCL_DETERMINISTICO, parametro_beta);
    , CANT_ITERS_MEDICION, &promedio_medicion);    
    
    g.establecer_camino_solucion(camino);
    if(g.hay_solucion()){
        cerr << g.obtener_cantidad_nodos() << " " << g.obtener_cantidad_aristas() << " " << CANT_ITERS_MEDICION << " " << promedio_medicion;
        #ifdef DEBUG_MESSAGES_ON
            cout << endl << "Solucion obtenida con golosa" << endl;
            camino.imprimir_camino(cout);
        #endif

    }

    //cout << "Salida del algoritmo: " << endl;
    g.serialize(cout, FORMATO_1_N_CLOSED);
    //cout << endl;
}