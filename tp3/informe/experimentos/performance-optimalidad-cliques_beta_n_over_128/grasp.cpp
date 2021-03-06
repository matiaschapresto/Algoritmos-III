#include "grafo.h"
#include "timing.h"
#include <fstream>
#include <cmath>

#define FILE_ITERS_MEJORA "evolucion_iteraciones_grasp.txt"
#define FILE_ITERS_COSTOS_ABSOLUTOS "costos_absolutos_iteraciones_grasp.txt"
#define FILE_ITERS_COSTOS_ABSOLUTOS_STATISTICS "costos_absolutos_iteraciones_grasp_analisis.txt"

void ejecutar_grasp(Grafo &g);

// -------------- Main ---------------------------------
int main(int argc, char **argv){
    list<Grafo> instancias = Grafo::parsear_varias_instancias(FORMATO_1_N_CLOSED);
    uint64_t instance_number = 1;
    for(Grafo &g : instancias){
        #ifdef DEBUG_MESSAGES_ON
            cout << endl << endl << "Aplicando metaheuristica GRASP a la " << instance_number << "-esima instancia de input..." << endl;
        #endif
        ejecutar_grasp(g);
        instance_number++;
    }
    return 0;
}

void ejecutar_grasp(Grafo &g){
    #ifdef DEBUG_MESSAGES_ON
        //g.imprimir_lista_adyacencia(cout);
        //g.imprimir_matriz_adyacencia(cout);
    #endif

//----- Configuracion del criterio de terminacion de GRASP -----

    //typedef enum criterio_terminacion_grasp_t {CRT_K_ITERS_SIN_MEJORA, CRT_K_ITERS_LIMIT_REACHED} criterio_terminacion_grasp_t;
    criterio_terminacion_grasp_t criterio_terminacion = CRT_K_ITERS_SIN_MEJORA;
    //este parametro denota la cantidad de iteraciones maxima, dependiendo del tipo de criterio, de cantidad fija de iteraciones o cantidad de iters
    //consecutivas sin mejora
    uint64_t ITERS_LIMIT = 5;
    //consecutivas sin que greedy rand me de una solucion factible
    uint64_t RAND_GREEDY_BAD_ITERS_LIMIT = 3;
    //este parametro denota el valor aceptable de la funcion objetivo w2 a partir del cual, dejamos de mejorar la solucion y consideramos que es lo suficientemente buena

//----- Configuracion de los modos de la busqueda local y golosa -----

    //typedef enum tipo_ejecucion_bqlocal_t {BQL_SUBDIVIDIR_PARES, BQL_CONTRAER_TRIPLAS_A_PARES, BQL_MEJORAR_CONEXION_TRIPLAS, BQL_COMBINAR} tipo_ejecucion_bqlocal_t;
    tipo_ejecucion_bqlocal_t modo_busqueda_local = BQL_COMBINAR;
    //typedef enum tipo_ejecucion_golosa_t {RCL_DETERMINISTICO, RCL_POR_VALOR, RCL_POR_CANTIDAD} tipo_ejecucion_golosa_t;
    tipo_ejecucion_golosa_t modo_golosa = RCL_POR_CANTIDAD;
    //si el tipo de golosa es RCL_POR_VALOR, este parametro indica el porcentaje de alejamiento del minimo de los candidatos de la lista
    //mas formalmente filtra todos los candidatos factibles locales que no cumplan candidato->costo_w2 <= valor_limite
    //donde valor limite es  (parametro_beta + 1) * minimo.second.obtener_costo_w2();
    //si el tipo de golosa es RCL_POR_CANTIDAD, este parametro indica la cantidad min{cant_candidatos, parametro_beta} de soluciones a considerar en la lista
    //si el tipo es RCL_DETERMINISTICO, este parametro es ignorado por el metodo.    
    double parametro_beta = ceil(g.obtener_cantidad_nodos()/(float)128);
    if(modo_golosa == RCL_POR_CANTIDAD){
        assert(parametro_beta >= 1);        
    }else if(modo_golosa == RCL_POR_VALOR){
        assert(parametro_beta > 0);
    }

    //-------------------------------------------------------

    bool condicion_terminacion = false;
    Camino mejor_solucion = g.obtener_camino_vacio();
    costo_t costo_mejor_solucion = costo_infinito;
    uint64_t cant_iters = 0;
    uint64_t cant_iters_sin_sol_greedy_rand_factible = 0;
    uint64_t cant_iters_sin_mejora = 0;
    double tiempo_golosa_randomized = 0;
    double promedio = 0;
    Camino camino = g.obtener_camino_vacio();
    bool sol_valida_greedy = false;
    vector<pair<uint, costo_t> > mejora_iters_grasp;
    vector<pair<costo_t,costo_t> > costo_camino_en_iteraciones;//costos w1, w2

    do{
        tiempo_golosa_randomized=0;
         MEDIR_TIEMPO_PROMEDIO(
            camino = g.obtener_solucion_golosa_randomizada(modo_golosa, parametro_beta);                
            //cout << "Solucion inicial de la greedy:" << endl;
            //camino.imprimir_camino(cout);
        , 1, &tiempo_golosa_randomized);

        //la sol greedy rand a veces da cosas no factibles, asi que verifico:
        //g.hay_solucion() nos indica si existe una sol factible(greedy rand lo setea en false si el minimo dijktra sobre w1 > limit_w1)
        //(camino.obtener_costo_total_w1_camino() <= g.obtener_limite_w1()); para chequear la validez del camino final de greedy rand
        sol_valida_greedy = (camino.obtener_costo_total_w1_camino() <= g.obtener_limite_w1());
        if(g.hay_solucion()){
            if(sol_valida_greedy){//puede que la greedy randomized no encuentre solucion!
                //hago iteraciones de busqueda local hasta que no haya mejora(la funcion devuelve true si hubo mejora, false sino)   
                g.establecer_camino_solucion(camino);

                //reseteo el contador de sol malas greedy rand
                cant_iters_sin_sol_greedy_rand_factible=0;

                int mejora_current_iteration = 0;
                uint64_t cant_iters_bqlocal = 0;
                double promedio_parcial_bqlocal = 0;
                double promedio_bqlocal = 0;
                do{
                    promedio_parcial_bqlocal = 0;
                    MEDIR_TIEMPO_PROMEDIO(
                        mejora_current_iteration = g.busqueda_local(modo_busqueda_local);
                        , 1, &promedio_parcial_bqlocal);
                    cant_iters_bqlocal++;
                    promedio_bqlocal += promedio_parcial_bqlocal;            
                }while(mejora_current_iteration > 0);
                promedio_bqlocal = promedio_bqlocal /(double) cant_iters_bqlocal;

                //el tiempo de esta iteracion es la greedy randomized + bqlocal sobre esa sol inicial
                promedio += tiempo_golosa_randomized;
                promedio += promedio_bqlocal;
                
                //en este punto la bqlocal mejoro todo lo que pudo la sol. inicial obtenida con la randomized greedy
                //me fijo si esta solucion es mejor que la que tenia guardada, de ser asi actualizo el maximo y guardo la sol actual como la nueva mejor.
                camino = g.obtener_camino_solucion();
                costo_t costo_solucion_actual = camino.obtener_costo_total_w2_camino();

                //almaceno el costo total en esta iteracion
                if(costo_solucion_actual < costo_mejor_solucion){
                    //guardamos que en esta iteracion se encontro una mejora
                    if(cant_iters>0){//sino la primera vez pone infinito(costo_mejor_solucion es infinito al inicializar)
                        mejora_iters_grasp.push_back(make_pair(cant_iters, costo_mejor_solucion - costo_solucion_actual));                      
                    }
                    costo_mejor_solucion = costo_solucion_actual;
                    mejor_solucion = g.obtener_camino_solucion();
                    //reseteo el contador
                    cant_iters_sin_mejora = 0;
                }else{
                    //una iteracion consecutiva mas sin mejora
                    //guardamos que en esta iteracion se encontro una mejora
                    if(cant_iters>0){//sino la primera vez pone infinito(costo_mejor_solucion es infinito al inicializar)
                        mejora_iters_grasp.push_back(make_pair(cant_iters, 0));                    
                        
                    }
                    cant_iters_sin_mejora++;
                }
                //costo de la mejor sol guardada
                costo_t costo_w1_mejor_solucion = mejor_solucion.obtener_costo_total_w1_camino();
                costo_t costo_w2_mejor_solucion = mejor_solucion.obtener_costo_total_w2_camino();
                costo_camino_en_iteraciones.push_back(make_pair(costo_w1_mejor_solucion, costo_w2_mejor_solucion));
                cant_iters++;
            }else{
                cant_iters_sin_sol_greedy_rand_factible++;
                //si ya no tengo mas chanches, modifico los parametros de la metaheuristica
                if(cant_iters_sin_sol_greedy_rand_factible >= RAND_GREEDY_BAD_ITERS_LIMIT){
                    //si es rcl por cantidad bajo el parametro beta
                    if(modo_golosa == RCL_POR_CANTIDAD){
                        if(parametro_beta>2){
                            parametro_beta--;
                        }else{
                            //cout << "[GRASP] Golosa deterministica seteada." << endl;
                            parametro_beta=1;
                            modo_golosa = RCL_DETERMINISTICO;
                        }
                    }else if(modo_golosa == RCL_POR_VALOR){
                        //si es rcl por valor, seteo greedy deterministica
                        //es muy delicado ajustar el porcentaje adaptativamente!
                        parametro_beta = 0; //es lo mismo que poner golosa deterministico!
                        modo_golosa = RCL_DETERMINISTICO;
                    }
                    //cerr << "[GRASP] Greedy randomized dio sol mala. Cambiando parametro_beta: " << parametro_beta << endl;
                    cant_iters_sin_sol_greedy_rand_factible = 0;
                }
            }
        }else{
            break;//no hay solucion factible
        }
        if(criterio_terminacion == CRT_K_ITERS_LIMIT_REACHED){
            condicion_terminacion = (cant_iters < ITERS_LIMIT);
        }else if(criterio_terminacion == CRT_K_ITERS_SIN_MEJORA){
            condicion_terminacion = (cant_iters_sin_mejora < ITERS_LIMIT);
        }
    }while(condicion_terminacion);
    promedio = promedio / (double) cant_iters;

    //imprimo mediciones en stderr
    if(g.hay_solucion()){
        cerr << g.obtener_cantidad_nodos() << " " << g.obtener_cantidad_aristas() << " " << cant_iters << " " << promedio;        
        //mejora en iteraciones
        ofstream evolucion_iteraciones;
        evolucion_iteraciones.open(FILE_ITERS_MEJORA);
        for(auto element : mejora_iters_grasp){
            evolucion_iteraciones << element.first << " " << element.second << endl;
        }
        evolucion_iteraciones.close();

        //evolucion absoluta
        evolucion_iteraciones.open(FILE_ITERS_COSTOS_ABSOLUTOS);        
        for(uint i=1;i<=costo_camino_en_iteraciones.size();i++){
            //imprimr <iteracion> <costo_w2> <costo_w1>
            evolucion_iteraciones << i << " " << costo_camino_en_iteraciones[i-1].second << " " << costo_camino_en_iteraciones[i-1].first << endl;
        }
        evolucion_iteraciones.close();

        costo_t costo_w2_inicial = costo_camino_en_iteraciones.front().second;
        costo_t costo_w2_final = costo_camino_en_iteraciones.back().second;
        costo_t mejora_total_costo_w2 = costo_w2_inicial - costo_w2_final;

        evolucion_iteraciones.open(FILE_ITERS_COSTOS_ABSOLUTOS_STATISTICS);
            evolucion_iteraciones << costo_w2_inicial << " " << costo_w2_final << " " << mejora_total_costo_w2;
        evolucion_iteraciones.close();
    }

    g.serialize(cout, FORMATO_1_N_CLOSED);
}