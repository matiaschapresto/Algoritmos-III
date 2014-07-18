#include "grafo.h"
#include "timing.h"
#include <fstream>
#define FILE_ITERS_MEJORA "evolucion_iteraciones.txt"

void ejecutar_busqueda_local(Grafo &g);

// -------------- Main ---------------------------------
int main(int argc, char **argv){
    list<Grafo> instancias = Grafo::parsear_varias_instancias(FORMATO_1_N_CLOSED);
    uint64_t instance_number = 1;
    for(Grafo &g : instancias){
        #ifdef DEBUG_MESSAGES_ON
            cout << endl << endl << "Aplicando busqueda local a la " << instance_number << "-esima instancia de input..." << endl;
        #endif
        ejecutar_busqueda_local(g);
        instance_number++;
    }
    return 0;
}

void ejecutar_busqueda_local(Grafo &g){
//    #ifdef DEBUG_MESSAGES_ON
//        g.imprimir_lista_adyacencia(cout);
//        //g.imprimir_matriz_adyacencia(cout);
//    #endif

    costo_t limit_w1 = g.obtener_limite_w1();
    nodo_t nodo_src = g.obtener_nodo_origen();
    nodo_t nodo_dst = g.obtener_nodo_destino();
    
    //--------------------------------- Busco solucion inicial ----------------------

    vector<costo_t> costo_minimo;
    vector<nodo_t> predecesor;
    g.dijkstra(nodo_src, COSTO_W1, costo_minimo, predecesor);
    
    costo_t costo_src_dst = costo_minimo[nodo_dst];//costo(src, dst)

    //--------------------------------- Valido la factibilidad de la solucion----------------
    #ifdef DEBUG_MESSAGES_ON
        cout << "Se requiere un camino entre (" << nodo_src << ") y (" << nodo_dst<< ") que no exceda el costo " << limit_w1;
    #endif
    if(costo_src_dst == costo_infinito){
        cerr << "No existe solucion factible. No existe camino entre origen(" << nodo_src << ") y destino(" << nodo_dst << ") " << endl;
        g.establecer_se_encontro_solucion(false);
    }else if(costo_src_dst > limit_w1){
        cerr << "No existe solucion factible. El camino minimo respecto a w1 de origen(" << nodo_src << ") a destino(" << nodo_dst << ") es de costo " << costo_src_dst << endl;
        g.establecer_se_encontro_solucion(false);
    }else{
        //armar camino entre origen y destino y lo establezco como sol inicial
        Camino c = g.obtener_camino_vacio();
        nodo_t nodo = nodo_dst;
        do{
            //cout << nodo << " " ;
            c.agregar_nodo_adelante(nodo);
            nodo = predecesor[nodo];
        }while(nodo != predecesor_nulo);
        g.establecer_camino_solucion(c);

        //imprimo sol inicial.
        #ifdef DEBUG_MESSAGES_ON
            cout << ". Costo minimo obtenido: " << c.obtener_costo_total_w1_camino();
            cout << "...Ok!!" << endl;
            cout << "Camino inicial: ";
            c.imprimir_camino(cout);
            cout << endl;
        #endif

        //--------------------------------- Comienzo la busqueda local -------------------   
        //typedef enum tipo_ejecucion_bqlocal_t {BQL_SUBDIVIDIR_PARES, BQL_CONTRAER_TRIPLAS_A_PARES, BQL_MEJORAR_CONEXION_TRIPLAS, BQL_COMBINAR} tipo_ejecucion_bqlocal_t;
        tipo_ejecucion_bqlocal_t tipo_ejecucion = BQL_COMBINAR;

        //hago iteraciones de busqueda local hasta que no haya mejora(la funcion devuelve true si hubo mejora, false sino)
        int mejora_current_iteration = 0;
        uint64_t cant_iters = 0;
        double promedio_parcial = 0;
        double promedio = 0;
        vector<costo_t> mejora_en_iteraciones;
        do{
            promedio_parcial = 0;
            MEDIR_TIEMPO_PROMEDIO(
                mejora_current_iteration = g.busqueda_local(tipo_ejecucion);
                , 1, &promedio_parcial);
            cant_iters++;
            promedio += promedio_parcial;            
            mejora_en_iteraciones.push_back(mejora_current_iteration);
        }while(mejora_current_iteration > 0);
        promedio = promedio /(double) cant_iters;

        #ifdef DEBUG_MESSAGES_ON
            switch(tipo_ejecucion){
                case BQL_SUBDIVIDIR_PARES:
                    cout << "Finalizo la busqueda local insertando entre pares porque no se obtuvieron nuevas mejoras." << endl;        
                    break;
                case BQL_CONTRAER_TRIPLAS_A_PARES:
                    cout << "Finalizo la busqueda local salteando entre triplas porque no se obtuvieron nuevas mejoras." << endl;
                    break;
                case BQL_MEJORAR_CONEXION_TRIPLAS:
                    cout << "Finalizo la busqueda local reemplazando entre triplas porque no se obtuvieron nuevas mejoras." << endl;
                    break;
                case BQL_COMBINAR:
                    cout << "Finalizo la busqueda local combinada porque no se obtuvieron nuevas mejoras." << endl;
                    break;                    
            }
        #endif
        g.establecer_se_encontro_solucion(true);
        cerr << g.obtener_cantidad_nodos() << " " << g.obtener_cantidad_aristas() << " " << cant_iters << " " << promedio;
        
        //mejora en iteraciones
        ofstream evolucion_iteraciones;
        evolucion_iteraciones.open(FILE_ITERS_MEJORA);
        for(uint i=1;i<=mejora_en_iteraciones.size();i++){
            evolucion_iteraciones << i << " " << mejora_en_iteraciones[i-1] << endl;
        }
        evolucion_iteraciones.close();
    }

    g.serialize(cout, FORMATO_1_N_CLOSED);
}
