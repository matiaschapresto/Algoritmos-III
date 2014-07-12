// Grafo Headers

typedef int nodo_t;
typedef int longuitud_t;
typedef double distancia_t;
typedef double costo_t;
typedef enum tipo_costo_t {COSTO_W1, COSTO_W2} tipo_costo_t;
typedef enum tipo_ejecucion_golosa_t {RCL_DETERMINISTICO, RCL_POR_VALOR, RCL_POR_CANTIDAD} tipo_ejecucion_golosa_t;
typedef enum tipo_ejecucion_bqlocal_t {BQL_SUBDIVIDIR_PARES, BQL_CONTRAER_TRIPLAS_A_PARES, BQL_MEJORAR_CONEXION_TRIPLAS} tipo_ejecucion_bqlocal_t;
typedef enum criterio_terminacion_grasp_t {CRT_K_ITERS_SIN_MEJORA, CRT_K_ITERS_LIMIT_REACHED, CRT_SOLUTION_GOOD_ENOUGH} criterio_terminacion_grasp_t;
typedef enum formato_entrada_t {FORMATO_0_N_OPEN, FORMATO_1_N_CLOSED} formato_entrada_t;
typedef formato_entrada_t formato_salida_t;

const costo_t costo_infinito = numeric_limits<double>::infinity();
const distancia_t distancia_infinita = numeric_limits<double>::infinity();
const costo_t costo_nulo = 0;
const nodo_t predecesor_nulo = -1;

class Arista{
private:
	bool presente;
	costo_t costo_w1;
	costo_t costo_w2;
public:
	Arista();
	Arista(bool pres, costo_t w1, costo_t w2);
	~Arista();

	bool esta_presente();
	void desmarcar_presente();
	void marcar_presente(costo_t w1, costo_t w2);
	costo_t obtener_costo_w1() const;
	costo_t obtener_costo_w2() const;

	bool operator== (const Arista &other) const
	{
	    return (other.presente == this->presente &&
	            other.costo_w1 == this->costo_w1 &&
	            other.costo_w2 == this->costo_w2);
	}
};

typedef vector<vector<Arista> > matriz_adyacencia_t;
typedef list<pair<nodo_t, Arista> > lista_adyacentes;
typedef vector<lista_adyacentes> lista_adyacencia_t;

class Vecino{
private:
	nodo_t i;
	nodo_t j;
	nodo_t en_comun;
	Arista desde_i_a_comun;
	Arista desde_j_a_comun;
public:
	Vecino(nodo_t i, nodo_t j, nodo_t comun, Arista desde_i, Arista desde_j);
	Vecino();
	~Vecino();
	nodo_t obtener_nodo_i();
	nodo_t obtener_nodo_j();
	nodo_t obtener_nodo_comun();
	Arista obtener_arista_i_comun();
	Arista obtener_arista_j_comun();
};

class Camino{
private:
	list<nodo_t> camino;
	vector<bool> esta_en_camino;
	costo_t costo_camino_w1;
	costo_t costo_camino_w2;
	matriz_adyacencia_t mat_adyacencia;
public:
	//Camino();
	Camino(matriz_adyacencia_t& mat_adyacencia);	
	~Camino();

	void agregar_nodo(nodo_t target);
	void agregar_nodo_adelante(nodo_t target);
	costo_t obtener_costo_total_w1_camino();
	costo_t obtener_costo_total_w2_camino();
	//pre: 0 <= i <= j < cantidad_nodos y que i,j sean adyacentes
	costo_t obtener_costo_w1_entre_nodos(nodo_t i, nodo_t j);
	//pre: 0 <= i <= j < cantidad_nodos y que i,j sean adyacentes
	costo_t obtener_costo_w2_entre_nodos(nodo_t i, nodo_t j);

	void imprimir_camino(ostream& out);

	list<nodo_t>::iterator obtener_iterador_begin();
	list<nodo_t>::iterator obtener_iterador_end();
	list<nodo_t>::const_iterator obtener_iterador_const_begin();
	list<nodo_t>::const_iterator obtener_iterador_const_end();
	longuitud_t obtener_longuitud_camino();

	//pre: at.obtener_nodo_i() y at.obtener_nodo_i() deben pertenecer al camino
	//Se reemplazara la conexion directa entre i y j por i -> encomun -> j indicado por el Vecino pasado
	//por parametro. Devuelve true si se inserto, false sino.
	bool insertar_nodo(Vecino& at);

	//pre: at.obtener_nodo_i() y at.obtener_nodo_i() deben pertenecer al camino
	//Se reemplazara la conexion i..loquesea..j por i -> encomun -> j indicado por el Vecino pasado
	//por parametro. Devuelve true si se inserto, false sino.
	bool mejorar_tripla(Vecino& at);

	bool realizar_salto_entre_3_nodos(nodo_t punto_de_salto);
	bool pertenece_a_camino(nodo_t target);//O(1)
};

class Grafo{
private:
	//atributos
	matriz_adyacencia_t mat_adyacencia;
	lista_adyacencia_t lista_adyacencia;
	int cantidad_nodos;
	int cantidad_aristas;
	
	//atributos propios del problema
	nodo_t nodo_src;
	nodo_t nodo_dst;
	costo_t cota_w1;
	Camino camino_obtenido;
	bool sol_valida;
	//dado el camino, podemos obtener los pesos de cada "salto" indexando en la matriz de adyacencia el costo de cada salto
	//tanto para w1 como w2

	//metodos auxiliares
	bool mejorar_conexion_entre_pares(nodo_t nodo_i, nodo_t nodo_j, costo_t costo_ij_w1, costo_t costo_ij_w2, costo_t total_w1, costo_t total_w2,
	 Vecino& mejor_vecino);

	bool mejorar_conexion_salteando(nodo_t nodo_i, nodo_t nodo_j, costo_t costo_ij_w1, costo_t costo_ij_w2, costo_t total_w1, costo_t total_w2,
	 Arista& mejor_vecino);

	//Golosa
	vector<pair<nodo_t, Arista> > obtener_lista_restringida_candidatos(nodo_t actual, double parametro_beta, vector<costo_t>& costos,
	vector<distancia_t>& distancias, costo_t costoCamino, distancia_t distanciaLlegada, tipo_ejecucion_golosa_t tipo_ejecucion);

	bool busqueda_local_entre_pares_insertando();
	bool busqueda_local_entre_triplas_salteando();
	bool busqueda_local_entre_triplas_reemplazando_intermedio();
public:
	//constructor y destructor
	Grafo(int cant_inicial_nodos);
	~Grafo();
	
	//Modificadores
	void agregar_nodos(int cantidad_nodos);
	void agregar_arista(nodo_t i, nodo_t j, costo_t w1, costo_t w2);
	void quitar_arista(nodo_t i, nodo_t j);

	//Consultas
	vector<Arista> obtener_vector_fila_vecinos(nodo_t target);
	lista_adyacentes obtener_lista_vecinos(nodo_t target);
	int obtener_cantidad_nodos();
	int obtener_cantidad_aristas();
	Arista obtener_arista(nodo_t i, nodo_t j);	
	//pre: 0 <= i <= j < cantidad_nodos y que i,j sean adyacentes
	list<Vecino> obtener_adyacentes_en_comun(nodo_t i, nodo_t j);
	nodo_t obtener_nodo_origen();
	nodo_t obtener_nodo_destino();
	costo_t obtener_limite_w1();
	Camino obtener_camino_solucion();
	void establecer_camino_solucion(Camino c);

	//Entrada - Salida
	void imprimir_matriz_adyacencia(ostream& out);
	void imprimir_lista_adyacencia(ostream& out);
	void serialize(ostream& out, formato_salida_t formato);
	bool unserialize(istream& in, formato_entrada_t formato);

	//Algoritmos
	//Realiza la busqueda local sobre una solucion inicial factible creada por dijkstra sobre COSTO_W1 entre src y dst
	bool busqueda_local(tipo_ejecucion_bqlocal_t tipo_ejecucion);
	//Devuelve el camino minimo entre origen y destino(calcula el arbol, pero reconstruye solo el camino de origen a destino)
	Camino dijkstra(nodo_t origen, nodo_t destino, tipo_costo_t target_a_minimizar);
	//Aplica dijkstra desde nodo origen y calcula el arbol de caminos minimos por referencia a los vectores por parametro
	void dijkstra(nodo_t origen, nodo_t nodo_origen_a_ignorar, tipo_costo_t target_a_minimizar, vector<costo_t>& costo_minimo, vector<nodo_t>& predecesor);
	//Dado un nodo_t origen se calcula para cada nodo, la distancia minima en cantidad de aristas de peso constante 1 de cualquier nodo a origen
	void breadth_first_search(nodo_t origen, vector<distancia_t>& distancias_en_aristas_a_origen);

	//Golosa
	Camino obtener_solucion_golosa(tipo_ejecucion_golosa_t tipo_ejecucion, double parametro_beta);

	//Metodos utilitarios
	static list<Grafo> parsear_varias_instancias(formato_entrada_t formato);
	void establecer_se_encontro_solucion(bool se_encontro);
	bool hay_solucion();
	Camino obtener_camino_vacio();
};

#endif


//Grafo Impl

#include "grafo.h"
// -------------- Arista ---------------------------------

Arista::Arista(){
	this->presente = false;
	this->costo_w1 = 0;
	this->costo_w2 = 0;
}

Arista::Arista(bool pres, costo_t w1, costo_t w2){
	this->presente = pres;
	this->costo_w1 = w1;
	this->costo_w2 = w2;
}

Arista::~Arista(){

}

bool Arista::esta_presente(){
	return this->presente;
}

costo_t Arista::obtener_costo_w1() const {
	return this->costo_w1;	
}

costo_t Arista::obtener_costo_w2() const {
	return this->costo_w2;	
}

void Arista::marcar_presente(costo_t w1, costo_t w2){
	this->presente = true;
	this->costo_w1 = w1;
	this->costo_w2 = w2;
}

void Arista::desmarcar_presente(){
	this->presente = false;
	this->costo_w1 = 0;
	this->costo_w2 = 0;
}

// -------------- Camino ---------------------------------

Camino::Camino(matriz_adyacencia_t& mat_adyacencia){
	this->mat_adyacencia = mat_adyacencia;
	this->costo_camino_w1 = 0;
	this->costo_camino_w2 = 0;
	int cant_nodos = mat_adyacencia.size();
	this->esta_en_camino.resize(cant_nodos, false);
}

//Camino::Camino(){
//	this->costo_camino_w1 = 0;
//	this->costo_camino_w2 = 0;
//	int cant_nodos = ?;
//	this->esta_en_camino.resize(cant_nodos, false);
//	cerr << "Usar con ojo!. Construyendo camino sin lista de adyacencia!!!!!" << endl;
//}

Camino::~Camino(){

}

void Camino::agregar_nodo(nodo_t target){
	costo_t costo_agregado_w1 = 0;
	costo_t costo_agregado_w2 = 0;
	if(this->camino.size()>0){
		nodo_t penultimo_nodo = camino.back();
		costo_agregado_w1 = obtener_costo_w1_entre_nodos(penultimo_nodo, target);
		costo_agregado_w2 = obtener_costo_w2_entre_nodos(penultimo_nodo, target);
		this->costo_camino_w1 += costo_agregado_w1;	
		this->costo_camino_w2 += costo_agregado_w2;	
	}
	this->camino.push_back(target);
	this->esta_en_camino[target] = true;
}

void Camino::agregar_nodo_adelante(nodo_t target){
	costo_t costo_agregado_w1 = 0;
	costo_t costo_agregado_w2 = 0;
	if(this->camino.size()>0){
		nodo_t penultimo_nodo = camino.front();
		costo_agregado_w1 = obtener_costo_w1_entre_nodos(penultimo_nodo, target);
		costo_agregado_w2 = obtener_costo_w2_entre_nodos(penultimo_nodo, target);
		this->costo_camino_w1 += costo_agregado_w1;	
		this->costo_camino_w2 += costo_agregado_w2;	
	}
	this->camino.push_front(target);
	this->esta_en_camino[target] = true;
}

costo_t Camino::obtener_costo_total_w1_camino(){
	return this->costo_camino_w1;
}

costo_t Camino::obtener_costo_total_w2_camino(){
	return this->costo_camino_w2;
}

//pre: 0 <= i <= j < cantidad_nodos y que i,j sean adyacentes
costo_t Camino::obtener_costo_w1_entre_nodos(nodo_t i, nodo_t j){
	if(this->mat_adyacencia[i][j].esta_presente()){
		return this->mat_adyacencia[i][j].obtener_costo_w1();
	}else{
		cerr << "Costo w1 infinito(no son adyacentes) entre nodos (" << i << ") y (" << j << ")" << endl;
		return costo_infinito;
	}	
}

//pre: 0 <= i <= j < cantidad_nodos y que i,j sean adyacentes
costo_t Camino::obtener_costo_w2_entre_nodos(nodo_t i, nodo_t j){
	if(this->mat_adyacencia[i][j].esta_presente()){
		return this->mat_adyacencia[i][j].obtener_costo_w2();
	}else{
		cerr << "Costo w2 infinito(no son adyacentes) entre nodos (" << i << ") y (" << j << ")" << endl;
		return costo_infinito;
	}	
}

void Camino::imprimir_camino(ostream& out){
	if(this->camino.size() < 2){
		cerr << "[Imprimir camino] Error - Camino Invalido (size < 2)" << endl;
		return;
	}

	list<nodo_t>::iterator it = this->camino.begin();
	list<nodo_t>::iterator runner_it = this->camino.begin();
	runner_it++;	
	while(runner_it != this->camino.end()){
		out << "(" << *it << ")" << "----[" << this->obtener_costo_w1_entre_nodos(*it, *runner_it) << ", " << this->obtener_costo_w2_entre_nodos(*it, *runner_it) << "]---->";
		++it;
		++runner_it;
	}	
	out << "(" << this->camino.back() << ")" << endl;
}

list<nodo_t>::const_iterator Camino::obtener_iterador_const_begin(){
	return this->camino.begin();
}

list<nodo_t>::const_iterator Camino::obtener_iterador_const_end(){
	return this->camino.end();
}

list<nodo_t>::iterator Camino::obtener_iterador_begin(){
	return this->camino.begin();
}

list<nodo_t>::iterator Camino::obtener_iterador_end(){
	return this->camino.end();
}

longuitud_t Camino::obtener_longuitud_camino(){
	return this->camino.size();
}

bool Camino::realizar_salto_entre_3_nodos(nodo_t nodo_target){
	list<nodo_t>::iterator runner_it = this->camino.begin();
	list<nodo_t>::iterator final_it = this->camino.end();
	while(runner_it != final_it){
		if(*runner_it == nodo_target){
			break;
		}
		++runner_it;
	}

	if(runner_it == final_it){
		cerr << "Camino::realizar_salto_entre_3_nodos. No se encontro el nodo target pasado por parametro en el camino" << endl;
		return false;
	}

	//aca vale *runner_it == nodo_target

	nodo_t nodo_i = *runner_it;
	
	runner_it++;	
	list<nodo_t>::iterator deletion_target = runner_it;
	nodo_t nodo_intermedio_viejo = *runner_it;
	
	runner_it++;
	nodo_t nodo_j = *runner_it;

	costo_t costo_i_intermedio_w1 = obtener_costo_w1_entre_nodos(nodo_i, nodo_intermedio_viejo);
	costo_t costo_i_intermedio_w2 = obtener_costo_w2_entre_nodos(nodo_i, nodo_intermedio_viejo);

	costo_t costo_intermedio_j_w1 = obtener_costo_w1_entre_nodos(nodo_intermedio_viejo, nodo_j);
	costo_t costo_intermedio_j_w2 = obtener_costo_w2_entre_nodos(nodo_intermedio_viejo, nodo_j);

	costo_t costo_i_j_w1 = obtener_costo_w1_entre_nodos(nodo_i, nodo_j);
	costo_t costo_i_j_w2 = obtener_costo_w2_entre_nodos(nodo_i, nodo_j);

	//elimino el nodo intermedio entre i y j
	this->camino.erase(deletion_target);
	this->esta_en_camino[nodo_intermedio_viejo] = false;

	//actualizo los costos del camino
	this->costo_camino_w1 = (this->costo_camino_w1 - (costo_i_intermedio_w1 + costo_intermedio_j_w1) + costo_i_j_w1);
	this->costo_camino_w2 = (this->costo_camino_w2 - (costo_i_intermedio_w2 + costo_intermedio_j_w2) + costo_i_j_w2);

	return true;
}

bool Camino::pertenece_a_camino(nodo_t target){
	return this->esta_en_camino[target];
}

//pre: at.obtener_nodo_i() y at.obtener_nodo_i() deben pertenecer al camino
//Se reemplazara la conexion i..loquesea..j por i -> encomun -> j indicado por el Vecino pasado
//por parametro. Devuelve true si se inserto, false sino.
bool Camino::mejorar_tripla(Vecino& at){
	nodo_t nodo_target = at.obtener_nodo_i();
	nodo_t nodo_sig_sig_target = at.obtener_nodo_j();
	nodo_t nodo_intermedio = at.obtener_nodo_comun();
	costo_t i_comun_w1 = at.obtener_arista_i_comun().obtener_costo_w1();
	costo_t i_comun_w2 = at.obtener_arista_i_comun().obtener_costo_w2();
	costo_t j_comun_w1 = at.obtener_arista_j_comun().obtener_costo_w1();
	costo_t j_comun_w2 = at.obtener_arista_j_comun().obtener_costo_w2();
	nodo_t intermedio_viejo = -1;

	list<nodo_t>::iterator insertion_target = this->camino.begin();
	list<nodo_t>::iterator final_it = this->camino.end();
	while(insertion_target != final_it){
		if(*insertion_target == nodo_target){	
			insertion_target++;
			intermedio_viejo = *insertion_target;
			break;
		}
		++insertion_target;
	}

	if(insertion_target == final_it){
		cerr << "Camino::mejorar_tripla. No se encontro el nodo target pasado por parametro en el camino" << endl;
		return false;
	}

	//aca vale que el iterator it apunta a target + 1

	//elimino el intermedio viejo
	nodo_t nodo_intermedio_viejo = *insertion_target;
	insertion_target = this->camino.erase(insertion_target);
	this->esta_en_camino[nodo_intermedio_viejo] = false;

	//inserto el nodo en el medio
	this->camino.insert(insertion_target, nodo_intermedio);
	this->esta_en_camino[nodo_intermedio] = true;

	//actualizo los costos
	//resto el costo entre i y j y sumo las 2 aristas nuevas

	costo_t costo_ij_w1 = obtener_costo_w1_entre_nodos(nodo_target, intermedio_viejo);
	costo_ij_w1 += obtener_costo_w1_entre_nodos(intermedio_viejo, nodo_sig_sig_target);

	costo_t costo_ij_w2 = obtener_costo_w2_entre_nodos(nodo_target, intermedio_viejo);
	costo_ij_w2 += obtener_costo_w2_entre_nodos(intermedio_viejo, nodo_sig_sig_target);
	            
    costo_t costo_i_comun_j_w1 = i_comun_w1 + j_comun_w1;
    costo_t costo_i_comun_j_w2 = i_comun_w2 + j_comun_w2;

	this->costo_camino_w1 = (this->obtener_costo_total_w1_camino() - costo_ij_w1 + costo_i_comun_j_w1);
	this->costo_camino_w2 = (this->obtener_costo_total_w2_camino() - costo_ij_w2 + costo_i_comun_j_w2);
	
	return true;
}

//pre: at.obtener_nodo_i() y at.obtener_nodo_i() deben pertenecer al camino
//Se reemplazara la conexion directa entre i y j por i -> encomun -> j indicado por el Vecino pasado
//por parametro
bool Camino::insertar_nodo(Vecino& at){
	nodo_t nodo_target = at.obtener_nodo_i();
	nodo_t nodo_sig_target = at.obtener_nodo_j();
	nodo_t nodo_intermedio = at.obtener_nodo_comun();
	costo_t i_comun_w1 = at.obtener_arista_i_comun().obtener_costo_w1();
	costo_t i_comun_w2 = at.obtener_arista_i_comun().obtener_costo_w2();
	costo_t j_comun_w1 = at.obtener_arista_j_comun().obtener_costo_w1();
	costo_t j_comun_w2 = at.obtener_arista_j_comun().obtener_costo_w2();

	list<nodo_t>::iterator insertion_target = this->camino.begin();
	list<nodo_t>::iterator final_it = this->camino.end();
	while(insertion_target != final_it){
		if(*insertion_target == nodo_target){
			insertion_target++;
			break;
		}
		++insertion_target;
	}

	if(insertion_target == final_it){
		cerr << "Camino::insertar_nodo. No se encontro el nodo target pasado por parametro en el camino" << endl;
		return false;
	}

	//aca vale que el iterator it apunta a target + 1

	//inserto el nodo en el medio
	this->camino.insert(insertion_target, nodo_intermedio);
	this->esta_en_camino[nodo_intermedio] = true;
	
	//actualizo los costos
	//resto la arista entre i y j y sumo las 2 aristas nuevas
	costo_t costo_ij_w1 = obtener_costo_w1_entre_nodos(nodo_target, nodo_sig_target);
	costo_t costo_ij_w2 = obtener_costo_w2_entre_nodos(nodo_target, nodo_sig_target);
	            
    costo_t costo_i_comun_j_w1 = i_comun_w1 + j_comun_w1;
    costo_t costo_i_comun_j_w2 = i_comun_w2 + j_comun_w2;

	this->costo_camino_w1 = (this->obtener_costo_total_w1_camino() - costo_ij_w1 + costo_i_comun_j_w1);
	this->costo_camino_w2 = (this->obtener_costo_total_w2_camino() - costo_ij_w2 + costo_i_comun_j_w2);
	return true;
}
// -------------- Vecino ---------------------------------

Vecino::Vecino(nodo_t i, nodo_t j, nodo_t comun, Arista desde_i, Arista desde_j){
	this->i = i;
	this->j = j;
	this->en_comun = comun;
	this->desde_i_a_comun = desde_i;
	this->desde_j_a_comun = desde_j;
}

Vecino::Vecino(){
	this->i = 0;
	this->j = 0;
	this->en_comun = 0;
	//aristas constructor por defecto
}

Vecino::~Vecino(){

}

nodo_t Vecino::obtener_nodo_i(){
	return this->i;
}

nodo_t Vecino::obtener_nodo_j(){
	return this->j;
}

nodo_t Vecino::obtener_nodo_comun(){
	return this->en_comun;
}

Arista Vecino::obtener_arista_i_comun(){
	return this->desde_i_a_comun;
}

Arista Vecino::obtener_arista_j_comun(){
	return this->desde_j_a_comun;
}

// -------------- Grafo ---------------------------------

Camino Grafo::obtener_camino_vacio(){
	Camino c(this->mat_adyacencia);
	return c;
}

Grafo::Grafo(int cant_inicial_nodos) : camino_obtenido(mat_adyacencia) {
	this->cantidad_nodos=cant_inicial_nodos;
	this->cantidad_aristas = 0;
	nodo_src = 0;
	nodo_dst = 0;
	cota_w1 = 0;
	sol_valida = false;
		
	//inicializo matriz de adyacencia
	vector<Arista> vec_fila(cantidad_nodos, Arista(false, 0, 0));
	this->mat_adyacencia.resize(cantidad_nodos, vec_fila);
	this->lista_adyacencia.resize(cantidad_nodos);
}

Grafo::~Grafo(){

}

//modificadores
void Grafo::agregar_nodos(int cantidad_nodos_nuevos){
	//agrego cantidad_nodos_nuevos columnas en todas las filas existentes
	for(int i=0;i < this->cantidad_nodos;i++){
		this->mat_adyacencia[i].resize(this->cantidad_nodos + cantidad_nodos_nuevos, Arista(false, 0, 0));		
	}
	
	//agrego cantidad_nodos_nuevos filas al final de la matriz de adyacencia
	vector<Arista> vec_fila(this->cantidad_nodos + cantidad_nodos_nuevos, Arista(false, 0, 0));
	this->mat_adyacencia.resize(this->cantidad_nodos + cantidad_nodos_nuevos, vec_fila);
	
	//redimensiono lista adyacencias
	this->lista_adyacencia.resize(cantidad_nodos + cantidad_nodos_nuevos);

	//actualizo cantidad_nodos total del grafo
	this->cantidad_nodos+=cantidad_nodos_nuevos;
}

void Grafo::agregar_arista(nodo_t i, nodo_t j, costo_t w1, costo_t w2){
	Arista arista = obtener_arista(i, j);
	//marco doble, no es un digrafo
	this->mat_adyacencia[i][j].marcar_presente(w1, w2);
	this->mat_adyacencia[j][i].marcar_presente(w1, w2);

	this->lista_adyacencia[i].push_back(make_pair(j, obtener_arista(i, j)));
	this->lista_adyacencia[j].push_back(make_pair(i, obtener_arista(i, j)));

	if(!arista.esta_presente()){
		this->cantidad_aristas++;
	}
}

void Grafo::quitar_arista(nodo_t i, nodo_t j){
	Arista arista = obtener_arista(i, j);
	//desmarco doble, no es un digrafo
	this->mat_adyacencia[i][j].desmarcar_presente();
	this->mat_adyacencia[j][i].desmarcar_presente();

	this->lista_adyacencia[i].remove(make_pair(j, arista));
	this->lista_adyacencia[j].remove(make_pair(i, arista));

	if(arista.esta_presente()){
		this->cantidad_aristas--;
	}	
}

//consultas
Arista Grafo::obtener_arista(nodo_t i, nodo_t j){
	return this->mat_adyacencia[i][j];
}

void Grafo::imprimir_matriz_adyacencia(ostream& out){	
	out << "Cantidad nodos: " << this->cantidad_nodos << endl;
	out << "Cantidad aristas: " << this->cantidad_aristas << endl;
	out << "Matriz adyacencia:" << endl;
	for (int i = 0;i<this->cantidad_nodos;i++){
		for (int j = 0;j<this->cantidad_nodos;j++){
			out << "|_(" << this->mat_adyacencia[i][j].esta_presente() << ")_|";
		}
		out << endl;
	}
	out << endl;
	out << endl;
}

void Grafo::imprimir_lista_adyacencia(ostream& out){	
	out << "Cantidad nodos: " << this->cantidad_nodos << endl;
	out << "Cantidad aristas: " << this->cantidad_aristas << endl;
	out << "Lista adyacencia:" << endl;
	for (int i = 0;i<this->cantidad_nodos;i++){
		out << "Vecinos de [" << i << "]: ";
		lista_adyacentes::iterator adyacentes_i_it = this->lista_adyacencia[i].begin();
		lista_adyacentes::iterator final_it = this->lista_adyacencia[i].end();
		while(adyacentes_i_it != final_it){
			out << "(" << adyacentes_i_it->first << ") --(" << this->mat_adyacencia[i][adyacentes_i_it->first].obtener_costo_w1() << ", " << this->mat_adyacencia[i][adyacentes_i_it->first].obtener_costo_w2() << ")--> ";
			adyacentes_i_it++;
		}
		out << "Nil" << endl;
	}
	out << endl;
	out << endl;
}

costo_t Grafo::obtener_limite_w1(){
	return this->cota_w1;
}

void Grafo::establecer_camino_solucion(Camino c){
	this->camino_obtenido = c;
}

Camino Grafo::obtener_camino_solucion(){
	return this->camino_obtenido;
}

int Grafo::obtener_cantidad_nodos(){
	return this->cantidad_nodos;
}

int Grafo::obtener_cantidad_aristas(){
	return this->cantidad_aristas;
}

void Grafo::serialize(ostream& out, formato_salida_t formato){
	if(this->sol_valida){
		out << this->camino_obtenido.obtener_costo_total_w1_camino() << " ";
		out << this->camino_obtenido.obtener_costo_total_w2_camino() << " ";
		out << this->camino_obtenido.obtener_longuitud_camino() << " ";
		list<nodo_t>::const_iterator it = this->camino_obtenido.obtener_iterador_const_begin();
		while(it != camino_obtenido.obtener_iterador_const_end()){
			out << ( (formato == FORMATO_1_N_CLOSED) ? ( (*it) + 1 ) : *it ) << " ";
			++it;
		}		
	}else{
		out << "no";
	}
}

bool Grafo::unserialize(istream& in, formato_entrada_t formato){
	//primera linea:
	//n m u v K
	//cant nodos, cant aristas, src, dst, K(cota w1)
	int cant_nodos_nuevos = 0, cantidad_aristas_nuevas = 0;
	in >> cant_nodos_nuevos;

	if(cant_nodos_nuevos == 0){
		return false;//es la ultima linea de la entrada!!
	}

	in >> cantidad_aristas_nuevas;
	in >> this->nodo_src;
	in >> this->nodo_dst;
	in >> this->cota_w1;

	if(formato == FORMATO_1_N_CLOSED){
		this->nodo_src--;
		this->nodo_dst--;
	}

	this->agregar_nodos(cant_nodos_nuevos);

	int count = 0;
	while(count < cantidad_aristas_nuevas){
		//las lineas de las aristas vienen asi:
		// v1 v2 w1 w2
		nodo_t nodo_a = 0, nodo_b = 0; costo_t costo_w1 = 0, costo_w2 = 0;
		in >> nodo_a;
		in >> nodo_b;
		in >> costo_w1;
		in >> costo_w2;

		if(formato == FORMATO_1_N_CLOSED){
			nodo_a--;
			nodo_b--;
		}

		this->agregar_arista(nodo_a, nodo_b, costo_w1, costo_w2);
		count++;
	}
	return true;
}

//Devuelve el camino minimo entre origen y destino(calcula el arbol, pero reconstruye solo el camino de origen a destino)
Camino Grafo::dijkstra(nodo_t origen, nodo_t destino, tipo_costo_t target_a_minimizar){
	vector<costo_t> costo_minimo;
	vector<nodo_t> predecesor;
	this->dijkstra(origen, -1, target_a_minimizar, costo_minimo, predecesor);
	
	//armar camino entre origen y destino
	Camino c(this->mat_adyacencia);
	nodo_t nodo = destino;
	do{
		//cout << nodo << " " ;
		c.agregar_nodo_adelante(nodo);
		nodo = predecesor[nodo];
	}while(nodo != predecesor_nulo);
	return c;
}

//Aplica dijkstra desde nodo origen y calcula el arbol de caminos minimos por referencia a los vectores por parametro
void Grafo::dijkstra(nodo_t origen, nodo_t nodo_origen_a_ignorar, tipo_costo_t target_a_minimizar, vector<costo_t>& costo_minimo, vector<nodo_t>& predecesor){
	//inicializacion
	int n = this->cantidad_nodos;
	costo_minimo.clear();
	costo_minimo.resize(n, costo_infinito);
	costo_minimo[origen] = costo_nulo;

	predecesor.clear();
	predecesor.resize(n, predecesor_nulo);

	set<pair<costo_t, nodo_t> > cola;
	cola.insert(make_pair(costo_minimo[origen], origen));

	while(!cola.empty()){
		costo_t cost_to_u = cola.begin()->first;
		nodo_t nodo_u = cola.begin()->second;
		//marcar como visto en este caso es eliminarlo de la cola
		cola.erase(cola.begin());

		//Para cada vecino de u, obtengo el vector fila de la matriz de adyacencia
		//de dicho nodo e itero sobre todos, quedandome con las aristas presentes			

//		Implementacion con lista de adyacencia
		lista_adyacentes::iterator adyacentes_i_it = this->lista_adyacencia[nodo_u].begin();
		lista_adyacentes::iterator final_it = this->lista_adyacencia[nodo_u].end();
		while(adyacentes_i_it != final_it){
			nodo_t nodo_v = adyacentes_i_it->first;


			//como se explica en el informe, para la sol golosa
			//queremos ignorar todos los caminos que pasen por cierto nodo
			//de esta forma me aseguro que dijkstra nunca va a considerar
			//mejorar un camino(o armarlo) pasando por este nodo.
			if(nodo_v == nodo_origen_a_ignorar){
				adyacentes_i_it++;
				continue;
			};


			costo_t cost_v = (target_a_minimizar == COSTO_W1) ? (adyacentes_i_it->second).obtener_costo_w1() : (adyacentes_i_it->second).obtener_costo_w2();
			int costo_a_v_pasando_por_u = cost_to_u + cost_v;
			//si mejora, sobreescribo el camino y su costo
			if(costo_a_v_pasando_por_u < costo_minimo[nodo_v]){
				//elimino de la cola de nodos la distancia vieja
				cola.erase(make_pair(costo_minimo[nodo_v], nodo_v));
				
				//actualizo estructuras
				costo_minimo[nodo_v] = costo_a_v_pasando_por_u;
				predecesor[nodo_v] = nodo_u;

				//sobreescribo la distancia en la cola de nodos la distancia nueva
				cola.insert(make_pair(costo_minimo[nodo_v], nodo_v));
			}
			adyacentes_i_it++;
		}

//		Implementacion con matriz de adyacencia
//		nodo_t vecino_candidato = 0;
//		vector<Arista> vecinos = this->mat_adyacencia[nodo_u];
//		while(vecino_candidato < this->cantidad_nodos){
//			if(vecinos[vecino_candidato].esta_presente()){
//				//hacer algo con este vecino
//				nodo_t nodo_v = vecino_candidato;
//				costo_t cost_v = (target_a_minimizar == COSTO_W1) ? vecinos[nodo_v].obtener_costo_w1() : vecinos[nodo_v].obtener_costo_w2();
//				int costo_a_v_pasando_por_u = cost_to_u + cost_v;
//				//si mejora, sobreescribo el camino y su costo
//				if(costo_a_v_pasando_por_u < costo_minimo[nodo_v]){
//					//elimino de la cola de nodos la distancia vieja
//					cola.erase(make_pair(costo_minimo[nodo_v], nodo_v));
//					
//					//actualizo estructuras
//					costo_minimo[nodo_v] = costo_a_v_pasando_por_u;
//					predecesor[nodo_v] = nodo_u;
//
//					//sobreescribo la distancia en la cola de nodos la distancia nueva
//					cola.insert(make_pair(costo_minimo[nodo_v], nodo_v));
//				}
//			}
//			vecino_candidato++;
//		}
	}
}

void Grafo::breadth_first_search(nodo_t origen, vector<distancia_t>& distancias){
	distancias.clear();
	distancias.resize(this->cantidad_nodos, distancia_infinita);
	queue<nodo_t> cola;
	vector<bool> visitado(this->cantidad_nodos, false);//inicializo todos los nodos sin visitar
	cola.push(origen);
	visitado[origen] = true;
	distancias[origen] = 0;//dist(src, src) = 0

	while(!cola.empty()){
		nodo_t target = cola.front();//obtengo el primero
		cola.pop();//desencolo el primero

		//para todos los vecinos de target
//		Implementacion con lista de adyacencia
		lista_adyacentes::iterator adyacentes_i_it = this->lista_adyacencia[target].begin();
		lista_adyacentes::iterator final_it = this->lista_adyacencia[target].end();
		while(adyacentes_i_it != final_it){
			nodo_t vecino_candidato = adyacentes_i_it->first;
			if(!visitado[vecino_candidato]){
				cola.push(vecino_candidato);
				visitado[vecino_candidato] = true;
				distancias[vecino_candidato] = distancias[target] + 1;
			}
			adyacentes_i_it++;
		}

//		Implementacion con matriz de adyacencia		
//		nodo_t vecino_candidato = 0;
//		vector<Arista> vecinos = this->mat_adyacencia[target];
//		while(vecino_candidato < this->cantidad_nodos){
//			if(vecinos[vecino_candidato].esta_presente()){
//				if(!visitado[vecino_candidato]){
//					cola.push(vecino_candidato);
//					visitado[vecino_candidato] = true;
//					distancias[vecino_candidato] = distancias[target] + 1;
//				}
//			}
//			vecino_candidato++;
//		}
	}
}

//para impl. con listas de adyacencia, es O(n**2) si no estan ordenados haciendo 2 fors
//se puede mejorar, "ordenando" las listas en espacios auxiliares en O(nlogn) y intersecando en O(n)
//dejando complejidad de O(nlogn), de cualquier manera, como se nos pidio que fuera polinomial unicamente
//uso la matriz de adyacencia, y en O(n) recorro los vecinos de ambos, y donde se cumpla vecindad en ambos
//lo selecciono como vecino en comun.
list<Vecino> Grafo::obtener_adyacentes_en_comun(nodo_t i, nodo_t j){
	list<Vecino> res;
	vector<Arista> adyacentesFila_i = this->mat_adyacencia[i];
	vector<Arista> adyacentesFila_j = this->mat_adyacencia[j];
	for(int idx=0;idx<this->cantidad_nodos;idx++){
		if(adyacentesFila_i[idx].esta_presente() && adyacentesFila_j[idx].esta_presente()){
			//el nodo idx es adyacente de i y j.
			res.push_back(Vecino(i, j, idx, adyacentesFila_i[idx], adyacentesFila_j[idx]));
		}
	}	
	return res;
}

vector<Arista> Grafo::obtener_vector_fila_vecinos(nodo_t target){
	return this->mat_adyacencia[target];
}

lista_adyacentes Grafo::obtener_lista_vecinos(nodo_t target){
	return this->lista_adyacencia[target];
}

nodo_t Grafo::obtener_nodo_origen(){
	return this->nodo_src;
}

nodo_t Grafo::obtener_nodo_destino(){
	return this->nodo_dst;
}

bool Grafo::mejorar_conexion_salteando(nodo_t nodo_i, nodo_t nodo_j, costo_t costo_ij_w1, costo_t costo_ij_w2, costo_t total_w1, costo_t total_w2,
 Arista& mejor_vecino){
	//es un grafo no dirigido, es simetrico buscar si a i es adyacente a j o viceversa
	//me fijo si i y j son adyacentes
	Arista candidato_a_mejor_camino = this->obtener_arista(nodo_i, nodo_j);
	if(candidato_a_mejor_camino.esta_presente()){
		//si asigno candidato_a_mejor_camino estoy devolviendo una referencia a una variable de stack y catapunchis!
		mejor_vecino = this->obtener_arista(nodo_i, nodo_j);
		return true;
	}
	return false;
}

bool Grafo::mejorar_conexion_entre_pares(nodo_t nodo_i, nodo_t nodo_j, costo_t costo_ij_w1, costo_t costo_ij_w2, costo_t total_w1, costo_t total_w2,
 Vecino& mejor_vecino){

	//SEA LA VECINDAD Vc = {Caminos C' tal que difieran de C en tan solo un nodo}

	//DEFINO UNA TABOO LIST COMO UNA LISTA EN LA CUAL VOY A DESCARTAR LAS SOLUCIONES QUE USEN NODOS DE DICHA LISTA
	//ESTO ES NECESARIO PARA EVITAR LA GENERACION DE CICLOS EN EL CASO DE AGREGAR UN NODO AL SUBDIVIDIR UNA ARISTA O REEMPLAZAR UN NODO
	//INTERMEDIO ENTRE OTROS DOS. SI EL NODO ELEGIDO YA PERTENECIA AL CAMINO, SE GENERAN CICLOS Y NO QUEREMOS ESTO YA QUE BUSCAMOS UN CAMINO MINIMO.
	//SI MANTENEMOS DISJUNTOS EL CONJUNTO DE NODOS DEL CAMINO ACTUAL Y LOS NODOS RESTANTES DEL GRAFO, CUALQUIER ELECCION QUE HAGAMOS NO GENERARA CICLOS.

	//OTRA OPCION SERIA NO RESTRINGIR LA ELECCION DE LAS SOLUCIONES DE LA VECINDAD, PERO DEBERIAMOS LUEGO REALIZAR UNA PODA DE CICLOS DEL CAMINO
	//CREEMOS QUE ESTA OPCION SERIA MEJOR, PORQUE AGREGANDO EL NODO SE MEJORA LA SOLUCION, Y ELIMINANDO EL CICLO, SE MEJORA AUN MAS, PERO A NUESTRO ENTENDER
	//DEJA DE SER BUSQUEDA LOCAL, DADO QUE LA SOLUCION QUE SURJA DE ESTO PUEDE NO ESTAR EN LA VECINDAD Vc

    //busco la conexion entre i y j pasando por un nodo intermedio tal que minimice la distancia de w2 sin pasarme de la cota total de w1 para el camino
 	list<Vecino> vecinosEnComun = this->obtener_adyacentes_en_comun(nodo_i, nodo_j);
	list<Vecino>::iterator vecinos_it = vecinosEnComun.begin();
	list<Vecino>::iterator final_vecinos = vecinosEnComun.end();
	
    //me fijo todos los caminos alternativos agregando un nodo entre los nodos ij,
    list<Vecino>::iterator mejor_vecino_it = vecinosEnComun.end();//inicializamos en algo que indique que no hay mejora
	costo_t mejor_camino_ij_w2 = costo_ij_w2;
	while(vecinos_it != final_vecinos){
        costo_t i_comun_w1 = vecinos_it->obtener_arista_i_comun().obtener_costo_w1();
        costo_t i_comun_w2 = vecinos_it->obtener_arista_i_comun().obtener_costo_w2();
        costo_t j_comun_w1 = vecinos_it->obtener_arista_j_comun().obtener_costo_w1();
        costo_t j_comun_w2 = vecinos_it->obtener_arista_j_comun().obtener_costo_w2();
        costo_t costo_i_comun_j_w1 = i_comun_w1 + j_comun_w1;
        costo_t costo_i_comun_j_w2 = i_comun_w2 + j_comun_w2;
		costo_t hipotetico_w1_total_camino = (total_w1 - costo_ij_w1 + costo_i_comun_j_w1);
		costo_t hipotetico_w2_total_camino = (total_w2 - costo_ij_w2 + costo_i_comun_j_w2);
			
		//veamos si el camino es una solucion factible
		if(hipotetico_w1_total_camino < this->obtener_limite_w1()){
			//es factible, veamos si mejora al ultimo mejor revisado

			//VEAMOS ADEMAS, QUE NO ESTE EN NUESTRA TABOO LIST(QUE NO PERTENEZCA AL CAMINO DE LA SOL. ACTUAL)
			//list<nodo_t>::iterator it_end = this->camino_obtenido.obtener_iterador_end();
			//list<nodo_t>::iterator found_it = this->camino_obtenido.obtener_iterador_begin();
			//nodo_t nodo_comun = vecinos_it->obtener_nodo_comun();
			//while((found_it != it_end) && (*found_it != nodo_comun)){
			//	found_it++;
			//}
			//if(found_it == it_end){

			//mejora: puedo verlo en O(1)
			nodo_t nodo_comun = vecinos_it->obtener_nodo_comun();
			if(!this->camino_obtenido.pertenece_a_camino(nodo_comun)){
				//el nodo no esta en la taboo list
				if(costo_i_comun_j_w2 < mejor_camino_ij_w2){
					//encontre mejora, actualizo variables
					mejor_camino_ij_w2 = hipotetico_w2_total_camino;
					mejor_vecino_it = vecinos_it;
				}				
			}//else{
				//taboo list skipped!
				//cout << "Salteamos el nodo (" << nodo_comun << ") como candidato a mejorar la solucion actual porque al pertenecer al camino generaria un ciclo" << endl;
			//}
		}
		++vecinos_it;
	}
	if(mejor_vecino_it != final_vecinos){
		mejor_vecino = *mejor_vecino_it;
		return true;
	}
	return false;
 }


//Defino la vecindad del camino como Vc = {Todos los caminos c' que difieren en un nodo de c}
//Intento mejorar un camino vk---->vk+1 con otro vk---->vj---->vk+1 tal que mejora w2 y w1 no se pasa en el costo total del camino
//Primero reviso todos los pares de nodos del camino buscando posibles subdivisiones que mejoren la solucion.
//Me voy a quedar unicamente (si existen varias) con la mejor solucion de la vecindad, notemos que revisar toda la vecindad es cuadratico
//La longuitud de un camino simple puede acotarse por la cantidad de nodos n, luego hay n-1 pares de nodos en el camino
//Para cada par, es lineal la obtencion de los vecinos en comun, y al ser el camino una lista enlazada, la modificacion tiene costo
//constante O(1), en total esto nos da un costo cuadratico O(n**2)
bool Grafo::busqueda_local_entre_pares_insertando(){
	//cout << "-------------------------------Comienza iteracion de busqueda local insertando entre pares--------------------------------" << endl;
	list<nodo_t>::const_iterator it = this->camino_obtenido.obtener_iterador_const_begin();
	list<nodo_t>::const_iterator runner_it = this->camino_obtenido.obtener_iterador_const_begin();
	runner_it++;
	list<nodo_t>::const_iterator final_camino = this->camino_obtenido.obtener_iterador_const_end();
	costo_t total_w1 = this->camino_obtenido.obtener_costo_total_w1_camino();
	costo_t total_w2 = this->camino_obtenido.obtener_costo_total_w2_camino();

	//variables para guardar la mejor solucion de la vecindad
	costo_t mejor_costo_w1 = costo_infinito;
	costo_t mejor_costo_w2 = costo_infinito;
	Vecino conexion_ij_minima_w2;
	list<nodo_t>::const_iterator punto_de_insercion_mejora_it = final_camino;
	bool hay_mejoras_para_el_camino = false;

	//busco la mejor solucion en la vecindad
	//cout << "Solucion actual: ";
	//this->camino_obtenido.imprimir_camino(cout);
    //cout << "Costo total del camino actual:   W1: " << total_w1 << "    W2: " << total_w2 << endl;
	while(runner_it != final_camino){
        nodo_t nodo_i = *it;
        nodo_t nodo_j = *runner_it;
        costo_t costo_ij_w1 = this->camino_obtenido.obtener_costo_w1_entre_nodos(nodo_i, nodo_j);
    	costo_t costo_ij_w2 = this->camino_obtenido.obtener_costo_w2_entre_nodos(nodo_i, nodo_j);

    	Vecino mejor_conexion_ij;
    	//le paso una ref a una var tipo vecino, si devuelve true, se escribe por referencia el mejor camino, sino, no cambia lo que le pasamos.
    	//cout << "Buscando mejorar la conexion (" << nodo_i << ")----[" << costo_ij_w1 << ", " << costo_ij_w2 << "]---->(" << nodo_j << ") agregando un nodo intermedio..." << endl;
        bool encontre_mejora = mejorar_conexion_entre_pares(nodo_i, nodo_j,
        										 costo_ij_w1, costo_ij_w2,
        										 total_w1, total_w2,
        										 mejor_conexion_ij);

		//hay que ver si encontramos una mejora
		if(encontre_mejora){//la funcion asegura que si dio true, me da el vecino por puntero en mejor_conexion_ij
            //nodo_t nodo_comun = mejor_conexion_ij.obtener_nodo_comun();
            costo_t i_comun_w1 = mejor_conexion_ij.obtener_arista_i_comun().obtener_costo_w1();
            costo_t i_comun_w2 = mejor_conexion_ij.obtener_arista_i_comun().obtener_costo_w2();
            costo_t j_comun_w1 = mejor_conexion_ij.obtener_arista_j_comun().obtener_costo_w1();
            costo_t j_comun_w2 = mejor_conexion_ij.obtener_arista_j_comun().obtener_costo_w2();
            costo_t costo_i_comun_j_w1 = i_comun_w1 + j_comun_w1;
            costo_t costo_i_comun_j_w2 = i_comun_w2 + j_comun_w2;
			//cout << "\tSe encontro una posible mejora. El mejor sendero entre los nodos que se encontro en todos los vecinos entre (" << nodo_i << ") y (" << nodo_j << ") es " << endl;
            //cout << "\tCamino (" << nodo_i << ")----[" << i_comun_w1 << ", " << i_comun_w2 <<  "]---->(" << nodo_comun << ")----[" << j_comun_w1 << ", " << j_comun_w2 << "]---->(" << nodo_j;
            //cout <<	") Nuevo costo del sendero entre (" << nodo_i << ") y (" << nodo_j << ") aplicando a esta modificacion:    W1: " << costo_i_comun_j_w1 << "     W2: " << costo_i_comun_j_w2 << endl;

			costo_t hipotetico_w1_total_camino = (total_w1 - costo_ij_w1 + costo_i_comun_j_w1);
			costo_t hipotetico_w2_total_camino = (total_w2 - costo_ij_w2 + costo_i_comun_j_w2);

			if( (hipotetico_w2_total_camino < mejor_costo_w2) && (hipotetico_w1_total_camino < this->obtener_limite_w1())){
				mejor_costo_w1 = hipotetico_w1_total_camino;
				mejor_costo_w2 = hipotetico_w2_total_camino;
				conexion_ij_minima_w2 = mejor_conexion_ij;
				punto_de_insercion_mejora_it = it;
				hay_mejoras_para_el_camino = true;
			}

			//cout << "\tSi aplicamos este cambio los costos del camino total quedarian:   W1: " << hipotetico_w1_total_camino << "    W2: "  << hipotetico_w2_total_camino << endl;
		}//else{
			//cout << "\tNo se encontro mejora." << endl;
		//}
        //cout << endl;
		++it;
		++runner_it;
	}

	//Si hubo mejoras, tengo guardada la mejor
	if(hay_mejoras_para_el_camino){
		//cout << "Se encontro mejora. La mejor mejora encontrada entre todos los pares de nodos fue:" << endl;
		//insertar conexion_ij_minima_w2 en punto_de_insercion_mejora_it
		//y actualizar todos los atributos necesarios.

		nodo_t nodo_i = conexion_ij_minima_w2.obtener_nodo_i();
		nodo_t nodo_j = conexion_ij_minima_w2.obtener_nodo_j();
		nodo_t nodo_comun = conexion_ij_minima_w2.obtener_nodo_comun();
        costo_t i_comun_w1 = conexion_ij_minima_w2.obtener_arista_i_comun().obtener_costo_w1();
        costo_t i_comun_w2 = conexion_ij_minima_w2.obtener_arista_i_comun().obtener_costo_w2();
        costo_t j_comun_w1 = conexion_ij_minima_w2.obtener_arista_j_comun().obtener_costo_w1();
        costo_t j_comun_w2 = conexion_ij_minima_w2.obtener_arista_j_comun().obtener_costo_w2();
        costo_t costo_i_comun_j_w1 = i_comun_w1 + j_comun_w1;
        costo_t costo_i_comun_j_w2 = i_comun_w2 + j_comun_w2;		
        //cout << "Se agregara un nodo intermedio en: (" << nodo_i << ")----[" << i_comun_w1 << ", " << i_comun_w2 <<  "]---->(" << nodo_comun << ")----[" << j_comun_w1 << ", " << j_comun_w2 << "]---->(" << nodo_j << ")" << endl;
        //cout <<	"Nuevo costo del sendero entre (" << nodo_i << ") y (" << nodo_j << ") aplicando a esta modificacion:    W1: " << costo_i_comun_j_w1 << "     W2: " << costo_i_comun_j_w2 << endl;
		if(!this->camino_obtenido.insertar_nodo(conexion_ij_minima_w2)){
			return false;
		}

        //cout << endl << "Nueva solucion obtenida: ";
        //this->camino_obtenido.imprimir_camino(cout);
		//cout << "Nuevos costos totales del camino:   W1: " << mejor_costo_w1 << "    W2: "  << mejor_costo_w2 << endl;		
	}else{
		//cout << "No se pudo mejorar la solucion." << endl;
	}
	return hay_mejoras_para_el_camino;
}

bool Grafo::busqueda_local_entre_triplas_reemplazando_intermedio(){
	//Caso en que reemplazo vk+1 por otro vecino comun vj, convirtiendo vk---->vk+1---->vk+2 en vk---->vj---->vk+2 tal que mejora w2 y w1 no se pasa en el costo total del camino	
	//cout << "-------------------------------Comienza iteracion de busqueda local reemplazando intermedio--------------------------------" << endl;
	if(this->camino_obtenido.obtener_longuitud_camino()<3){
		cerr << "Camino de menos de 3 nodos. No se puede mejorar nada." << endl;
		return false;
	}

	//aca vale size(camino)>=3

	list<nodo_t>::const_iterator it = this->camino_obtenido.obtener_iterador_const_begin();
	list<nodo_t>::const_iterator it_sig = this->camino_obtenido.obtener_iterador_const_begin();
	list<nodo_t>::const_iterator runner_it = this->camino_obtenido.obtener_iterador_const_begin();
	it_sig++;
	runner_it++;runner_it++;

	list<nodo_t>::const_iterator final_camino = this->camino_obtenido.obtener_iterador_const_end();
	costo_t total_w1 = this->camino_obtenido.obtener_costo_total_w1_camino();
	costo_t total_w2 = this->camino_obtenido.obtener_costo_total_w2_camino();

	//variables para guardar la mejor solucion de la vecindad
	costo_t mejor_costo_w1 = costo_infinito;
	costo_t mejor_costo_w2 = costo_infinito;
	Vecino conexion_ij_minima_w2;
	list<nodo_t>::const_iterator punto_de_insercion_mejora_it = final_camino;
	bool hay_mejoras_para_el_camino = false;

	//busco la mejor solucion en la vecindad
	//cout << "Solucion actual: ";
	//this->camino_obtenido.imprimir_camino(cout);
    //cout << "Costo total del camino actual:   W1: " << total_w1 << "    W2: " << total_w2 << endl;
	while(runner_it != final_camino){
        nodo_t nodo_i = *it;
        nodo_t nodo_medio = *it_sig;
        nodo_t nodo_j = *runner_it;

        costo_t costo_i_medio_w1 = this->camino_obtenido.obtener_costo_w1_entre_nodos(nodo_i, nodo_medio);
    	costo_t costo_i_medio_w2 = this->camino_obtenido.obtener_costo_w2_entre_nodos(nodo_i, nodo_medio);
        costo_t costo_medio_j_w1 = this->camino_obtenido.obtener_costo_w1_entre_nodos(nodo_medio, nodo_j);
    	costo_t costo_medio_j_w2 = this->camino_obtenido.obtener_costo_w2_entre_nodos(nodo_medio, nodo_j);


    	//cout << "Intentando mejorar el nodo intermedio entre (" << nodo_i << ")----[" << costo_i_medio_w1 << ", " << costo_i_medio_w2;
    	//cout << "]---->(" << nodo_medio << ")----[" << costo_medio_j_w1 << ", " << costo_medio_j_w2 << "]---->(" << nodo_j << ")" << endl;

    	Vecino mejor_conexion_ij;
    	//le paso una ref a una var tipo Vecino, si devuelve true, se escribe por referencia el mejor camino, sino, no cambia lo que le pasamos.
        bool encontre_mejora = mejorar_conexion_entre_pares(nodo_i, nodo_j,
        										 (costo_i_medio_w1 + costo_medio_j_w1), (costo_i_medio_w2 + costo_medio_j_w2), 
        										 total_w1, total_w2,
        										 mejor_conexion_ij);

		//hay que ver si encontramos una mejora
		if(encontre_mejora){//la funcion asegura que si dio true, me da el vecino por puntero en mejor_conexion_ij
            //nodo_t nodo_comun = mejor_conexion_ij.obtener_nodo_comun();
            costo_t i_comun_w1 = mejor_conexion_ij.obtener_arista_i_comun().obtener_costo_w1();
            costo_t i_comun_w2 = mejor_conexion_ij.obtener_arista_i_comun().obtener_costo_w2();
            costo_t j_comun_w1 = mejor_conexion_ij.obtener_arista_j_comun().obtener_costo_w1();
            costo_t j_comun_w2 = mejor_conexion_ij.obtener_arista_j_comun().obtener_costo_w2();
            costo_t costo_i_comun_j_w1 = i_comun_w1 + j_comun_w1;
            costo_t costo_i_comun_j_w2 = i_comun_w2 + j_comun_w2;
			//cout << "\tSe encontro una posible mejora. El mejor sendero entre los nodos que se encontro en todos los vecinos entre (" << nodo_i << ") y (" << nodo_j << ") es " << endl;
            //cout << "\tCamino (" << nodo_i << ")----[" << i_comun_w1 << ", " << i_comun_w2 <<  "]---->(" << nodo_comun << ")----[" << j_comun_w1 << ", " << j_comun_w2 << "]---->(" << nodo_j;
            //cout <<	") Nuevo costo del sendero entre (" << nodo_i << ") y (" << nodo_j << ") aplicando a esta modificacion:    W1: " << costo_i_comun_j_w1 << "     W2: " << costo_i_comun_j_w2 << endl;

			costo_t hipotetico_w1_total_camino = (total_w1 - (costo_i_medio_w1 + costo_medio_j_w1) + costo_i_comun_j_w1);
			costo_t hipotetico_w2_total_camino = (total_w2 - (costo_i_medio_w2 + costo_medio_j_w2) + costo_i_comun_j_w2);

			if( (hipotetico_w2_total_camino < mejor_costo_w2) && (hipotetico_w1_total_camino < this->obtener_limite_w1())){
				mejor_costo_w1 = hipotetico_w1_total_camino;
				mejor_costo_w2 = hipotetico_w2_total_camino;
				conexion_ij_minima_w2 = mejor_conexion_ij;
				punto_de_insercion_mejora_it = it;
				hay_mejoras_para_el_camino = true;
			}

			//cout << "\tSi aplicamos este cambio los costos del camino total quedarian:   W1: " << hipotetico_w1_total_camino << "    W2: "  << hipotetico_w2_total_camino << endl;
		}//else{
			//cout << "\tNo se encontro mejora." << endl;
		//}

        //cout << endl;
		++it;
		++it_sig;
		++runner_it;
	}

	//Si hubo mejoras, tengo guardada la mejor
	if(hay_mejoras_para_el_camino){
		//cout << "Se encontro mejora. La mejor mejora encontrada entre todos los pares de nodos fue:" << endl;
		//reemplazar conexion_ij_minima_w2 en punto_de_insercion_mejora_it y punto_de_insercion_mejora_it + 2
		//y actualizar todos los atributos necesarios.

		nodo_t nodo_i = conexion_ij_minima_w2.obtener_nodo_i();
		nodo_t nodo_j = conexion_ij_minima_w2.obtener_nodo_j();
		nodo_t nodo_comun = conexion_ij_minima_w2.obtener_nodo_comun();
        costo_t i_comun_w1 = conexion_ij_minima_w2.obtener_arista_i_comun().obtener_costo_w1();
        costo_t i_comun_w2 = conexion_ij_minima_w2.obtener_arista_i_comun().obtener_costo_w2();
        costo_t j_comun_w1 = conexion_ij_minima_w2.obtener_arista_j_comun().obtener_costo_w1();
        costo_t j_comun_w2 = conexion_ij_minima_w2.obtener_arista_j_comun().obtener_costo_w2();
        costo_t costo_i_comun_j_w1 = i_comun_w1 + j_comun_w1;
        costo_t costo_i_comun_j_w2 = i_comun_w2 + j_comun_w2;		
        //cout << "Se reemplazo el nodo intermedio en: (" << nodo_i << ")----[" << i_comun_w1 << ", " << i_comun_w2 <<  "]---->(" << nodo_comun << ")----[" << j_comun_w1 << ", " << j_comun_w2 << "]---->(" << nodo_j << ")" << endl;
        //cout <<	"Nuevo costo del sendero entre (" << nodo_i << ") y (" << nodo_j << ") aplicando a esta modificacion:    W1: " << costo_i_comun_j_w1 << "     W2: " << costo_i_comun_j_w2 << endl;

		if(!this->camino_obtenido.mejorar_tripla(conexion_ij_minima_w2)){
			return false;
		}

        //cout << endl << "Nueva solucion obtenida: ";
        //this->camino_obtenido.imprimir_camino(cout);
		//cout << "Nuevos costos totales del camino:   W1: " << mejor_costo_w1 << "    W2: "  << mejor_costo_w2 << endl;
			
	}else{
		//cout << "No se pudo mejorar la solucion." << endl;
	}
	return hay_mejoras_para_el_camino;
}

bool Grafo::busqueda_local_entre_triplas_salteando(){
	//Caso en los que salteo un nodo vk---->vk+1---->vk+2 convirtiendolo en vk---->vk+2 tal que mejora w2 y w1 no se pasa en el costo total del camino	
	//cout << "-------------------------------Comienza iteracion de busqueda local salteando--------------------------------" << endl;
	if(this->camino_obtenido.obtener_longuitud_camino()<3){
		cerr << "Camino de menos de 3 nodos. No se puede mejorar nada." << endl;
		return false;
	}

	//aca vale size(camino)>=3

	list<nodo_t>::const_iterator it = this->camino_obtenido.obtener_iterador_const_begin();
	list<nodo_t>::const_iterator it_sig = this->camino_obtenido.obtener_iterador_const_begin();
	list<nodo_t>::const_iterator runner_it = this->camino_obtenido.obtener_iterador_const_begin();
	it_sig++;
	runner_it++;runner_it++;

	list<nodo_t>::const_iterator final_camino = this->camino_obtenido.obtener_iterador_const_end();
	costo_t total_w1 = this->camino_obtenido.obtener_costo_total_w1_camino();
	costo_t total_w2 = this->camino_obtenido.obtener_costo_total_w2_camino();

	//variables para guardar la mejor solucion de la vecindad
	costo_t mejor_costo_w1 = costo_infinito;
	costo_t mejor_costo_w2 = costo_infinito;
	Arista conexion_ij_minima_w2;
	list<nodo_t>::const_iterator punto_de_salto_it = final_camino;
	bool hay_mejoras_para_el_camino = false;

	//busco la mejor solucion en la vecindad
	//cout << "Solucion actual: ";
	//this->camino_obtenido.imprimir_camino(cout);
    //cout << "Costo total del camino actual:   W1: " << total_w1 << "    W2: " << total_w2 << endl;
	while(runner_it != final_camino){
        nodo_t nodo_i = *it;
        nodo_t nodo_medio = *it_sig;
        nodo_t nodo_j = *runner_it;

        costo_t costo_i_medio_w1 = this->camino_obtenido.obtener_costo_w1_entre_nodos(nodo_i, nodo_medio);
    	costo_t costo_i_medio_w2 = this->camino_obtenido.obtener_costo_w2_entre_nodos(nodo_i, nodo_medio);
        costo_t costo_medio_j_w1 = this->camino_obtenido.obtener_costo_w1_entre_nodos(nodo_medio, nodo_j);
    	costo_t costo_medio_j_w2 = this->camino_obtenido.obtener_costo_w2_entre_nodos(nodo_medio, nodo_j);


    	//cout << "Intentando remover el nodo intermedio entre (" << nodo_i << ")----[" << costo_i_medio_w1 << ", " << costo_i_medio_w2;
    	//cout << "]---->(" << nodo_medio << ")----[" << costo_medio_j_w1 << ", " << costo_medio_j_w2 << "]---->(" << nodo_j << ")" << endl;

    	Arista mejor_conexion_ij;
    	//le paso una ref a una var tipo arista, si devuelve true, se escribe por referencia el mejor camino, sino, no cambia lo que le pasamos.
        bool encontre_mejora = mejorar_conexion_salteando(nodo_i, nodo_j,
        										 (costo_i_medio_w1 + costo_medio_j_w1), (costo_i_medio_w2 + costo_medio_j_w2), 
        										 total_w1, total_w2,
        										 mejor_conexion_ij);

		//hay que ver si encontramos una mejora
		if(encontre_mejora){//la funcion asegura que si dio true, me da el vecino por puntero en mejor_conexion_ij
			costo_t costo_ij_directo_w1 = mejor_conexion_ij.obtener_costo_w1();
			costo_t costo_ij_directo_w2 = mejor_conexion_ij.obtener_costo_w2();
			//cout << "\tSe encontro una posible mejora. Se encontro una arista directa entre (" << nodo_i << ") y (" << nodo_j << ") y es " << endl;
            //cout << "\tCamino (" << nodo_i << ")----[" << costo_ij_directo_w1 << ", " << costo_ij_directo_w2 <<  "]---->(" << nodo_j << ")";
            //cout <<	" Nuevo costo del sendero entre (" << nodo_i << ") y (" << nodo_j << ") aplicando a esta modificacion:    W1: " << costo_ij_directo_w1 << "     W2: " << costo_ij_directo_w2 << endl;

			costo_t hipotetico_w1_total_camino = (total_w1 - (costo_i_medio_w1 + costo_medio_j_w1) + costo_ij_directo_w1);
			costo_t hipotetico_w2_total_camino = (total_w2 - (costo_i_medio_w2 + costo_medio_j_w2) + costo_ij_directo_w2);

			if( (hipotetico_w2_total_camino < mejor_costo_w2) && (hipotetico_w1_total_camino < this->obtener_limite_w1())){
				mejor_costo_w1 = hipotetico_w1_total_camino;
				mejor_costo_w2 = hipotetico_w2_total_camino;
				hay_mejoras_para_el_camino = true;				
				conexion_ij_minima_w2 = mejor_conexion_ij;
				punto_de_salto_it = it;
			}

			//cout << "\tSi aplicamos este cambio los costos del camino total quedarian:   W1: " << hipotetico_w1_total_camino << "    W2: "  << hipotetico_w2_total_camino << endl;
		}

        //cout << endl;
		++it;
		++it_sig;
		++runner_it;
	}

	//Si hubo mejoras, tengo guardada la mejor
	if(hay_mejoras_para_el_camino){
		//cout << "Se encontro mejora. La mejor mejora encontrada entre todos los pares de nodos fue:" << endl;
		//Realizar el salto directo entre 2 nodos dada la arista obtenida
		list<nodo_t>::const_iterator nodo_j_it = punto_de_salto_it;
		nodo_j_it++;nodo_j_it++;

		nodo_t nodo_i = *punto_de_salto_it;
		nodo_t nodo_j = *nodo_j_it;

		costo_t costo_ij_directo_w1 = conexion_ij_minima_w2.obtener_costo_w1();
		costo_t costo_ij_directo_w2 = conexion_ij_minima_w2.obtener_costo_w2();
		//cout << "\tSe encontro una arista directa entre (" << nodo_i << ") y (" << nodo_j << ") y es " << endl;
        //cout << "\tCamino (" << nodo_i << ")----[" << costo_ij_directo_w1 << ", " << costo_ij_directo_w2 <<  "]---->(" << nodo_j << ")";
        //cout <<	" Nuevo costo del sendero entre (" << nodo_i << ") y (" << nodo_j << ") aplicando a esta modificacion:    W1: " << costo_ij_directo_w1 << "     W2: " << costo_ij_directo_w2 << endl;

		if(!this->camino_obtenido.realizar_salto_entre_3_nodos(nodo_i)){
			return false;
		}

		//No puede haber ciclos, porque el camino quedo igual o con menos nodos

        //cout << endl << "Nueva solucion obtenida: ";
        //this->camino_obtenido.imprimir_camino(cout);
		//cout << "Nuevos costos totales del camino:   W1: " << mejor_costo_w1 << "    W2: "  << mejor_costo_w2 << endl;
	}else{
		//cout << "No se pudo mejorar la solucion." << endl;
	}
	return hay_mejoras_para_el_camino;
}

bool Grafo::busqueda_local(tipo_ejecucion_bqlocal_t tipo_ejecucion){
	//typedef enum tipo_ejecucion_bqlocal_t {BQL_SUBDIVIDIR_PARES, BQL_CONTRAER_TRIPLAS_A_PARES, BQL_MEJORAR_CONEXION_TRIPLAS} tipo_ejecucion_bqlocal_t;
	switch(tipo_ejecucion){
		case BQL_SUBDIVIDIR_PARES:
			return busqueda_local_entre_pares_insertando();
		case BQL_CONTRAER_TRIPLAS_A_PARES:		
			return busqueda_local_entre_triplas_salteando();
		case BQL_MEJORAR_CONEXION_TRIPLAS:		
			return busqueda_local_entre_triplas_reemplazando_intermedio();
	}
	cerr << "busqueda_local: tipo ejecucion invalido!" << endl;
	return false;	
}

list<Grafo> Grafo::parsear_varias_instancias(formato_entrada_t formato){
	list<Grafo> instancias;
    //parseo todas las instancias
    bool instancia_valida = true;
    do{
        Grafo i(0);
        instancia_valida = i.unserialize(cin, formato);
        if(instancia_valida)
        	instancias.push_back(i);
    }while(instancia_valida);
    //cout << "[Parse input]Se leyeron " << instancias.size() << " instancias de stdin" << endl << endl;
    return instancias;
}

bool compare_w2(const pair<nodo_t, Arista>& i, const pair<nodo_t, Arista>& j)
{
    return (i.second).obtener_costo_w2() < (j.second).obtener_costo_w2();
}

//parametro beta en RCL_POR_VALOR indica el porcentaje a alejarse del mejor candidato
//parametro beta en RCL_POR_CANTIDAD indica la candidad de "mejores" candidatos a elegir
vector<pair<nodo_t, Arista> > Grafo::obtener_lista_restringida_candidatos(nodo_t actual, double parametro_beta,
	vector<costo_t>& costos, vector<distancia_t>& distancias, costo_t costo_camino, distancia_t distanciaLlegada, tipo_ejecucion_golosa_t tipo_ejecucion){
	
	vector<pair<nodo_t, Arista> > candidatos;
	
	//voy a filtrar todos los candidatos factibles localmente (que elegir dicho candidato no me pase de la cota de w1
	//y que me acerco a destino) y finalmente, segun el tipo de ejecucion voy a hacer una de las siguientes cosas:
	//	- RCL_DETERMINISTICO: tomo el minimo respecto a w2
	//  - RCL_POR_CANTIDAD: ordeno el vector segun w2 y resizeo a los primeros parametro_beta elementos. Esto me da los parametro_beta elementos mas chicos segun w2
	//	- RCL_POR_VALOR: ordeno el vector segun w2 y elimino los menores que (minimo_w2 * (1 + parametro_beta)). Esto me da los candidatos que se acercan parametro_beta % al minimo_w2

	lista_adyacentes vecinos = this->obtener_lista_vecinos(actual);	
	if(!vecinos.empty()){
		lista_adyacentes::iterator incidentes_i_it = vecinos.begin();
        lista_adyacentes::iterator final_it = vecinos.end();

        //cout << "Costo del camino parcial actual: " << costo_camino << endl;
		//recolecto los factibles locales        
        while(incidentes_i_it != final_it){
        	nodo_t vecino_actual = incidentes_i_it->first;
        	//cout << "Costo del vecino a destino (" << vecino_actual << "): " << costos[vecino_actual] << endl;
        	//cout << "Costo tentativo del camino parcial desde origen yendo por la direccion del vecino(" << vecino_actual << "): " << costos[vecino_actual] + costo_camino << endl;
        	bool no_me_paso_w1 = (costos[vecino_actual] + costo_camino <= this->cota_w1);
        	bool me_acerco_a_destino = (distancias[incidentes_i_it->first] < distanciaLlegada);
        	if(no_me_paso_w1 && me_acerco_a_destino){
                pair<nodo_t, Arista> target = *incidentes_i_it;
				candidatos.push_back(target);
            }
            incidentes_i_it++;
        }

        if(candidatos.size()==0){
        	cerr << "Lista de cantidatos factibles vacia (condicion requerida: no se pasa de w1 y se acerca a destino) en el nodo (" << actual << ")" << endl;
        	cerr << "Lista de nodos vecinos:" << endl;
        	lista_adyacentes::iterator incidentes_i_it = vecinos.begin();
        	lista_adyacentes::iterator final_it = vecinos.end();
        	while(incidentes_i_it != final_it){
	        	cerr << "(" << incidentes_i_it->first << ") costos w1: " << incidentes_i_it->second.obtener_costo_w1() << " costos w2:";
	        	cerr << incidentes_i_it->second.obtener_costo_w2() << endl;
	            incidentes_i_it++;
        	}
        	return candidatos;
        }else{
//        	cerr << "Lista de nodos factibles:" << endl;
//	    	vector<pair<nodo_t, Arista> >::iterator incidentes_i_it = candidatos.begin();
//	    	vector<pair<nodo_t, Arista> >::iterator final_it = candidatos.end();
//	    	while(incidentes_i_it != final_it){
//	        	cerr << "(" << incidentes_i_it->first << ") costos w1: " << incidentes_i_it->second.obtener_costo_w1() << " costos w2:";
//	        	cerr << incidentes_i_it->second.obtener_costo_w2() << endl;
//	            incidentes_i_it++;
//	    	}
        }

        //ordeno los candidatos factibles, sobre la funcion w2 de menor a mayor
        sort(candidatos.begin(), candidatos.end(), compare_w2);

        if(tipo_ejecucion == RCL_DETERMINISTICO){
        	//cout <<"RCL_DETERMINISTICO" << endl;
    		candidatos.resize(1);
    		//elimino todos salvo el minimo factible sobre w2
    	}else if(tipo_ejecucion == RCL_POR_VALOR){
    		//cout <<"RCL_POR_VALOR" << endl;
    		pair<nodo_t, Arista> minimo = candidatos.front();
    		double valor_limite = (parametro_beta + 1) * minimo.second.obtener_costo_w2();
    		vector<pair<nodo_t, Arista> >::iterator it = candidatos.begin();
    		vector<pair<nodo_t, Arista> >::iterator final_it = candidatos.end();

    		//filtro los que se pasen del valor limite
    		while(it != final_it){
    			if(it->second.obtener_costo_w2() > valor_limite){
    				it = candidatos.erase(it);
    				if(it == final_it){
    					//caso donde borro el ultimo, me queda final_it y no puedo hacerle ++ porque estalla
    					break;
    				}
    			}
    			it++;
    		}
    	}else if(tipo_ejecucion == RCL_POR_CANTIDAD){
    		//cout <<"RCL_POR_CANTIDAD" << endl;
			uint cantidad_trim = (int) parametro_beta;
			if (cantidad_trim < 1){
				cerr << "EH, EL PARAMETRO BETA POR CANTIDAD TOMANDO FLOOR ME DA MENOR QUE UNO, LE PONGO 1!!" << endl;
				cantidad_trim = 1;
			}
			if(cantidad_trim > candidatos.size()){
				//dejo solo los primeros cantidad_trim elementos mas chicos respecto a w2.
				candidatos.resize(cantidad_trim);				
			}
    	}
	}

//   	cerr << "Lista de nodos factibles filtrados:" << endl;
//   	vector<pair<nodo_t, Arista> >::iterator incidentes_i_it = candidatos.begin();
//   	vector<pair<nodo_t, Arista> >::iterator final_it = candidatos.end();
//   	while(incidentes_i_it != final_it){
//       	cerr << "(" << incidentes_i_it->first << ") costos w1: " << incidentes_i_it->second.obtener_costo_w1() << " costos w2:";
//       	cerr << incidentes_i_it->second.obtener_costo_w2() << endl;
//           incidentes_i_it++;
//   	}      
	
	return candidatos;
}

bool compare_w1(const pair<nodo_t, Arista>& i, const pair<nodo_t, Arista>& j)
{
    return (i.second).obtener_costo_w1() < (j.second).obtener_costo_w1();
}

//typedef enum tipo_ejecucion_golosa_t {RCL_DETERMINISTICO, RCL_POR_VALOR, RCL_POR_CANTIDAD} tipo_ejecucion_golosa_t;
Camino Grafo::obtener_solucion_golosa(tipo_ejecucion_golosa_t tipo_ejecucion, double parametro_beta){
	//---------------------------- Inicializo los vectores y datos ----------------
    nodo_t nodo_dst = obtener_nodo_destino();
    
    vector<costo_t> costos;
    vector<nodo_t> predecesores;
    //dijkstra IGNORANDO nodo_src
    this->dijkstra(nodo_dst, nodo_src, COSTO_W1, costos, predecesores);

    vector<distancia_t> distancias;
    this->breadth_first_search(nodo_dst, distancias);

    Camino camino(this->mat_adyacencia);
    camino.agregar_nodo(nodo_src);
    costo_t costo_camino = 0;
    distancia_t distanciaLlegada = distancias[nodo_src];

    if(distanciaLlegada == distancia_infinita){
    	cerr << "[Golosa] la distancia en aristas entre src y dst es infinita => no hay solucion." << endl;
    	//no hay camino entre src y dst => no hay solucion
    	this->establecer_se_encontro_solucion(false);
    	return camino;
    }else if(distanciaLlegada == 1){
    //Si distanciaLlegada == 1 entonces lo busco entre los vecinos y reviso esa arista para ver si w1(arista) > limit_w1
    	lista_adyacentes vecinos_src = obtener_lista_vecinos(nodo_src);
    	lista_adyacentes::iterator it = vecinos_src.begin();
    	lista_adyacentes::iterator final_it = vecinos_src.end();
    	while((it != final_it) && (it->first == nodo_dst)){
    		it++;
    	}
    	if(it == final_it){
    		cerr << "Distancia en aristas 1 y no son adyacentes src y dst...turbio" << endl;
    	}else{
    		costo_t limit_w1 = this->obtener_limite_w1();
    		if((it->second).obtener_costo_w1() > limit_w1){
    			cerr << "[Golosa] la distancia en costo entre src y dst es " << (it->second).obtener_costo_w1() << " > " << limit_w1 << " => no hay solucion." << endl;
		    	//el camino minimo se pasa de la cota => no hay solucion
		    	this->establecer_se_encontro_solucion(false);
		    	return camino;
    		}
    	}
    }else{//distanciaLlegada > 1
	    //como IGNORE nodo_src en dijkstra, la distancia va a ser distancia_infinita.
	    //pero quiero saber si el camino minimo sobre w1 es factible cuando distanciaLlegada>1
	    //entonces obtengo el minimo vecino sobre el peso w1 en la arista que los une
	    //y me fijo si dist(src, min_ady) +  dist(min_ady, dst) > K. Si esto vale
	    //el camino minimo sobre w1 entre src y dst se pasa de la cota, indicando que no hay sol.
	    //factible

		lista_adyacentes vecinos_src = obtener_lista_vecinos(nodo_src);

		pair<nodo_t, Arista > min_ady_pair = *min_element(vecinos_src.begin(), vecinos_src.end(), compare_w1);

	    nodo_t min_ady = min_ady_pair.first;
	    costo_t min_ady_src_w1 = (min_ady_pair.second).obtener_costo_w1();

	    distancia_t dist_cost_src_dst = costos[min_ady] + min_ady_src_w1;

	    costo_t limit_w1 = this->obtener_limite_w1();

	    if(dist_cost_src_dst > limit_w1){
	    	cerr << "[Golosa] la distancia en costo entre src y dst es " << dist_cost_src_dst << " > " << limit_w1 << " => no hay solucion." << endl;
	    	//el camino minimo se pasa de la cota => no hay solucion
	    	this->establecer_se_encontro_solucion(false);
	    	return camino;
	    }    	
	    //else{
	    	//cout << "Distancia en costo entre src y dst: " << dist_cost_src_dst << endl;
	    //}
	}
    //--------------------------Comienza la busqueda golosa ----------------

    //cout << "Distancia en aristas entre src y dst: " << distanciaLlegada << endl;
    nodo_t actual = nodo_src;
    //cout << "Nodo inicial: " << actual << endl;
    bool hubo_error = false;
    while(actual != nodo_dst){
       
        vector<pair<nodo_t, Arista> > candidatos = this->obtener_lista_restringida_candidatos(actual, parametro_beta, costos, distancias, costo_camino, distanciaLlegada, tipo_ejecucion);

        //cout << "Obteniendo mejor vecino del nodo (" << actual << ") segun decision greedy..." << endl;
        if(candidatos.empty()){
        	cerr << "[Golosa] Candidatos vacio. Cortando algoritmo, devolviendo camino parcial obtenido." << endl;
        	hubo_error = true;
        	break;
        }

        //Seleccion de decision greedy segun tipo de ejecucion requerido por parametro
        pair<nodo_t, Arista> minimo;
    	if(tipo_ejecucion == RCL_DETERMINISTICO){
    		//el metodo obtener_lista_restringida_candidatos me devuelve un unico elemento, el de la decision greedy 
			minimo = candidatos.front();        		        		
    	}else if(tipo_ejecucion == RCL_POR_VALOR || tipo_ejecucion == RCL_POR_CANTIDAD){
        	std::default_random_engine generator;
			//generacion numero random c++11 con distribucion uniforme
			random_device rd;
		    mt19937 gen(rd());
		    uniform_int_distribution<> dis(0, candidatos.size()-1);
			int candidato_random = dis(gen);
			//indexo con el numero random obtenido
			minimo = candidatos[candidato_random];   		
    	}

        //cout << "\tEl mejor vecino segun decision greedy para el nodo (" << actual << ") es (";
        //cout << minimo.first << ") con un costo (w1: " << (minimo.second).obtener_costo_w1() << ", w2: " << (minimo.second).obtener_costo_w2() << ")" << endl;
        //cout << endl;

        camino.agregar_nodo(minimo.first);
        costo_camino += (minimo.second).obtener_costo_w1();
        actual = minimo.first;
        distanciaLlegada--;
    }
    this->establecer_se_encontro_solucion(!hubo_error);
    return camino;
}

void Grafo::establecer_se_encontro_solucion(bool se_encontro){
	this->sol_valida = se_encontro;
}

bool Grafo::hay_solucion(){
	return this->sol_valida;
}

//Golosa

void ejecutar_greedy(Grafo &g){
    //g.imprimir_lista_adyacencia(cout);
    //g.imprimir_matriz_adyacencia(cout);
    double promedio_medicion = 0;
    double parametro_beta=1;
    //typedef enum tipo_ejecucion_golosa_t {RCL_DETERMINISTICO, RCL_POR_VALOR, RCL_POR_CANTIDAD} tipo_ejecucion_golosa_t;
    Camino camino = g.obtener_camino_vacio();
    MEDIR_TIEMPO_PROMEDIO(
                camino = g.obtener_solucion_golosa(RCL_DETERMINISTICO, parametro_beta);
    , CANT_ITERS_MEDICION, &promedio_medicion);    
    
    g.establecer_camino_solucion(camino);
    //El mismo metodo obtener_solucion_golosa establece si se encontro solucion!
    //g.establecer_se_encontro_solucion(true);
    if(g.hay_solucion()){
        cerr << g.obtener_cantidad_nodos() << " " << g.obtener_cantidad_aristas() << " " << CANT_ITERS_MEDICION << " " << promedio_medicion;
        //cout << endl << "Solucion obtenida con golosa" << endl;
        //camino.imprimir_camino(cout);
    }

    //cout << "Salida del algoritmo: " << endl;
    g.serialize(cout, FORMATO_1_N_CLOSED);
    //cout << endl;
}

//Busqueda local

void ejecutar_busqueda_local(Grafo &g){
    //g.imprimir_lista_adyacencia(cout);
    //g.imprimir_matriz_adyacencia(cout);

    costo_t limit_w1 = g.obtener_limite_w1();
    nodo_t nodo_src = g.obtener_nodo_origen();
    nodo_t nodo_dst = g.obtener_nodo_destino();
    
    //--------------------------------- Busco solucion inicial ----------------------
    Camino c = g.dijkstra(nodo_src, nodo_dst, COSTO_W1);
    g.establecer_camino_solucion(c);

    //--------------------------------- Valido la factibilidad de la solucion----------------
    //cout << "Se requiere un camino entre (" << nodo_src << ") y (" << nodo_dst<< ") que no exceda el costo " << limit_w1;
    if(c.obtener_costo_total_w1_camino() == costo_infinito){
        cerr << "No existe solucion factible. No existe camino entre origen(" << nodo_src << ") y destino(" << nodo_dst << ") " << endl;
        g.establecer_se_encontro_solucion(false);
    }else if(c.obtener_costo_total_w1_camino() > limit_w1){
        cerr << "No existe solucion factible. El camino minimo respecto a w1 de origen(" << nodo_src << ") a destino(" << nodo_dst << ") es de costo " << c.obtener_costo_total_w1_camino() << endl;
        g.establecer_se_encontro_solucion(false);
    }else{
        //cout << ". Costo minimo obtenido: " << c.obtener_costo_total_w1_camino();
        //cout << "...Ok!!" << endl;
        //cout << "Camino inicial: ";
        //c.imprimir_camino(cout);
        //cout << endl;

        //--------------------------------- Comienzo la busqueda local -------------------   
        //typedef enum tipo_ejecucion_bqlocal_t {BQL_SUBDIVIDIR_PARES, BQL_CONTRAER_TRIPLAS_A_PARES, BQL_MEJORAR_CONEXION_TRIPLAS} tipo_ejecucion_bqlocal_t;
        
        tipo_ejecucion_bqlocal_t tipo_ejecucion = BQL_SUBDIVIDIR_PARES;

        //hago iteraciones de busqueda local hasta que no haya mejora(la funcion devuelve true si hubo mejora, false sino)
        bool hay_mejora = false;
        uint64_t cant_iters = 0;
        double promedio_parcial = 0;
        double promedio = 0;
        do{
            MEDIR_TIEMPO_PROMEDIO(
                hay_mejora = g.busqueda_local(tipo_ejecucion);
                , CANT_ITERS_MEDICION, &promedio_parcial);
            cant_iters++;
            promedio += promedio_parcial;
        }while(hay_mejora);
        promedio = promedio /(double) cant_iters;

        switch(tipo_ejecucion){
            case BQL_SUBDIVIDIR_PARES:
                //cout << "Finalizo la busqueda local insertando entre pares porque no se obtuvo ninguna mejora." << endl;        
                break;
            case BQL_CONTRAER_TRIPLAS_A_PARES:
                //cout << "Finalizo la busqueda local salteando entre triplas porque no se obtuvo ninguna mejora." << endl;
                break;
            case BQL_MEJORAR_CONEXION_TRIPLAS:
                //cout << "Finalizo la busqueda local reemplazando entre triplas porque no se obtuvo ninguna mejora." << endl;
                break;
        }
        g.establecer_se_encontro_solucion(true);
        cerr << g.obtener_cantidad_nodos() << " " << g.obtener_cantidad_aristas() << " " << cant_iters << " " << promedio;
    }

    //cout << "Salida del algoritmo: " << endl;
    g.serialize(cout, FORMATO_1_N_CLOSED);
    //cout << endl;
}


//Grasp

void ejecutar_grasp(Grafo &g){
    //g.imprimir_lista_adyacencia(cout);
    //g.imprimir_matriz_adyacencia(cout);

//----- Configuracion del criterio de terminacion de GRASP -----

    //NOTA: CRT_SOLUTION_GOOD_ENOUGH SOLO TIENE SENTIDO SI PARA LA INSTANCIA SE CONOCE UNA COTA CONOCIDA de W2 que sea aceptable.    
    //USAR CON CUIDADO, O IGNORAR. PUEDE NO TERMINAR NUNCA, LO DEJO IMPLEMENTADO POR CUESTIONES DE EXPERIMENTACION NOMAS. ACLARAR EN EL INFORME!

    //NOTA: ESTO SOLO TIENE SENTIDO SI PARA LA INSTANCIA SE CONOCE UNA COTA CONOCIDA!
    //USAR CON CUIDADO, O IGNORAR. PUEDE NO TERMINAR NUNCA, LO DEJO IMPLEMENTADO POR CUESTIONES DE EXPERIMENTACION NOMAS. ACLARAR EN EL INFORME!
    costo_t W2_VALUE_TARGET = 5;

    //typedef enum criterio_terminacion_grasp_t {CRT_K_ITERS_SIN_MEJORA, CRT_K_ITERS_LIMIT_REACHED, CRT_SOLUTION_GOOD_ENOUGH} criterio_terminacion_grasp_t;
    criterio_terminacion_grasp_t criterio_terminacion = CRT_K_ITERS_SIN_MEJORA;
    //este parametro denota la cantidad de iteraciones maxima, dependiendo del tipo de criterio, de cantidad fija de iteraciones o cantidad de iters
    //consecutivas sin mejora
    uint64_t ITERS_LIMIT = 100;
    //este parametro denota el valor aceptable de la funcion objetivo w2 a partir del cual, dejamos de mejorar la solucion y consideramos que es lo suficientemente buena

//----- Configuracion de los modos de la busqueda local y golosa -----

    //typedef enum tipo_ejecucion_bqlocal_t {BQL_SUBDIVIDIR_PARES, BQL_CONTRAER_TRIPLAS_A_PARES, BQL_MEJORAR_CONEXION_TRIPLAS} tipo_ejecucion_bqlocal_t;
    //typedef enum tipo_ejecucion_golosa_t {RCL_DETERMINISTICO, RCL_POR_VALOR, RCL_POR_CANTIDAD} tipo_ejecucion_golosa_t;    
    tipo_ejecucion_bqlocal_t modo_busqueda_local = BQL_CONTRAER_TRIPLAS_A_PARES;
    tipo_ejecucion_golosa_t modo_golosa = RCL_POR_CANTIDAD;
    //si el tipo de golosa es RCL_POR_VALOR, este parametro indica el porcentaje de alejamiento del minimo de los candidatos de la lista
    //mas formalmente filtra todos los candidatos factibles locales que no cumplan candidato->costo_w2 <= valor_limite
    //donde valor limite es  (parametro_beta + 1) * minimo.second.obtener_costo_w2();
    //si el tipo de golosa es RCL_POR_CANTIDAD, este parametro indica la cantidad min{cant_candidatos, parametro_beta} de soluciones a considerar en la lista
    //si el tipo es RCL_DETERMINISTICO, este parametro es ignorado por el metodo.
    //double parametro_beta = 1.5;
    double parametro_beta = 2;

    //-------------------------------------------------------

    bool condicion_terminacion = false;
    Camino mejor_solucion = g.obtener_camino_vacio();
    costo_t costo_mejor_solucion = costo_infinito;
    uint64_t cant_iters = 0;
    uint64_t cant_iters_sin_mejora = 0;
    double tiempo_golosa_randomized = 0;
    double tiempo_bqlocal = 0;
    double promedio = 0;
    Camino camino = g.obtener_camino_vacio();
    bool sol_valida_greedy = false;
    uint64_t iters_golosa_bad_solution = 0;
    //ciclo greedy

    do{
        MEDIR_TIEMPO_PROMEDIO(
            camino = g.obtener_solucion_golosa(modo_golosa, parametro_beta);
            //cout << "Solucion inicial de la greedy:" << endl;
            //camino.imprimir_camino(cout);
        , 1, &tiempo_golosa_randomized);

        sol_valida_greedy = g.hay_solucion();
        if(sol_valida_greedy){//puede que la greedy randomized no encuentre solucion!
            //hago iteraciones de busqueda local hasta que no haya mejora(la funcion devuelve true si hubo mejora, false sino)   
            MEDIR_TIEMPO_PROMEDIO(
                g.establecer_camino_solucion(camino);
                while(g.busqueda_local(modo_busqueda_local));
            , 1, &tiempo_bqlocal);
            promedio += tiempo_golosa_randomized;
            promedio += tiempo_golosa_randomized;
            
            //en este punto la bqlocal mejoro todo lo que pudo la sol. inicial obtenida con la randomized greedy
            //me fijo si esta solucion es mejor que la que tenia guardada, de ser asi actualizo el maximo y guardo la sol actual como la nueva mejor.
            costo_t costo_solucion_actual = camino.obtener_costo_total_w2_camino();
            if(costo_solucion_actual < costo_mejor_solucion){
                costo_mejor_solucion = costo_solucion_actual;
                mejor_solucion = g.obtener_camino_solucion();
                //reseteo el contador
                cant_iters_sin_mejora = 0;
            }else{
                //una iteracion consecutiva mas sin mejora
                cant_iters_sin_mejora++;
            }
            cant_iters++;
            iters_golosa_bad_solution = 0;
        }else{
            iters_golosa_bad_solution++;
        }

        if(criterio_terminacion == CRT_K_ITERS_LIMIT_REACHED){
            condicion_terminacion = (cant_iters < ITERS_LIMIT);
        }else if(criterio_terminacion == CRT_K_ITERS_SIN_MEJORA){
            condicion_terminacion = (cant_iters_sin_mejora < ITERS_LIMIT);
        }else if(criterio_terminacion == CRT_SOLUTION_GOOD_ENOUGH){
            condicion_terminacion = (costo_mejor_solucion < W2_VALUE_TARGET);
        }
        condicion_terminacion = condicion_terminacion | (iters_golosa_bad_solution > GOLOSA_BAD_SOLUTION_CONSECUTIVAS_LIMIT);
    }while(!condicion_terminacion);
    promedio = promedio / (double) cant_iters;

    //imprimo mediciones en stderr
    cerr << g.obtener_cantidad_nodos() << " " << g.obtener_cantidad_aristas() << " " << cant_iters << " " << promedio;

    //cout << endl << "Se cumplio el criterio de terminacion elegido: ";
//    if(iters_golosa_bad_solution > GOLOSA_BAD_SOLUTION_CONSECUTIVAS_LIMIT){
//        cout << "GOLOSA_BAD_SOLUTION_CONSECUTIVAS_LIMIT";
//    }else if(criterio_terminacion == CRT_K_ITERS_LIMIT_REACHED){
//        cout << "CRT_K_ITERS_LIMIT_REACHED";
//    }else if(criterio_terminacion == CRT_K_ITERS_SIN_MEJORA){
//        cout << "CRT_K_ITERS_SIN_MEJORA";
//    }else if(criterio_terminacion == CRT_SOLUTION_GOOD_ENOUGH){
//        cout << "CRT_SOLUTION_GOOD_ENOUGH";
//    }
//    cout << endl;

    g.establecer_se_encontro_solucion(true);

    //cout << "Salida del algoritmo: " << endl;
    g.serialize(cout, FORMATO_1_N_CLOSED);
    //cout << endl;
}
