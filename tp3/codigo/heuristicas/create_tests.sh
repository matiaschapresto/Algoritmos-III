#!/bin/bash
echo "-------------------------------------------------------------------------------------------------------------------------"
echo "Compilando generador de tests-aleatorios..."
pushd	../generar_grafos/
	make clean all
popd
echo ""
echo "Creando tests aleatorios..."
echo "-------------------------------------------------------------------------------------------------------------------------"
red='\e[0;31m'
green='\e[0;32m'
NC='\e[0m' # No Color

cant_nodos_min=10
cant_nodos_max=15
peso_maximo_w1=50
peso_maximo_w2=50

for (( n=$cant_nodos_min; n<=$cant_nodos_max; n++ ))
do 
	aristas_minimas=$n
	aristas_maximas=$((n * (n-1)/2))
	echo "Generando grafos de $n nodos con cantidad de aristas aleatorias en el rango [$aristas_minimas .. $aristas_maximas]"
	echo "Los pesos W1 de las aristas van a estar en un rango preestablecido aleatorio entre [1..$peso_maximo_w1]"
	echo "Los pesos W2 de las aristas van a estar en un rango preestablecido aleatorio entre [1..$peso_maximo_w2]"
	for (( a=$aristas_minimas; a<=$aristas_maximas; a++ ))
	do
		echo -n "Generando grafo de $n nodos y $a aristas aleatorio..."
		#../generar_grafos/generar_grafos cantidad_nodos cantidad_aristas maximo_random_w1 maximo_random_w2
		../generar_grafos/generar_grafos "$n" "$a" "$peso_maximo_w1" "$peso_maximo_w2" > "test-cases/instancia_$n.$a.in"
		echo "Ok!"
	done
	echo "-------------------------------------------------------------------------------------------------------------------------"
done
echo "-------------------------------------------------------------------------------------------------------------------------"
pushd	../generar_grafos/
	make clean
popd