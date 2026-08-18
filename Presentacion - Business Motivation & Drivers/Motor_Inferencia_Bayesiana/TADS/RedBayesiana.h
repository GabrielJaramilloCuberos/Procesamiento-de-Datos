/*********************************************************************************
-Pontificia Universidad Javeriana
-Proyecto 3 - Introduccion a la Inteligencia Artificial
-Temas: TADs, Compilacion Modular, Redes Bayesianas
*********************************************************************************/
#ifndef REDBAYESIANA_H
#define REDBAYESIANA_H

#include <map>
#include <string>
#include <algorithm>
#include "VariableAleatoria.h"

using namespace std;

/**
 * @brief Representa una Red Bayesiana completa.
 *
 * Una Red Bayesiana es un grafo aciclico dirigido (DAG) donde cada nodo
 * es una VariableAleatoria. Esta clase gestiona la construccion de la red,
 * la carga de tablas de probabilidad condicional (CPTs) y el motor de
 * inferencia por enumeracion exacta (algoritmo ENUMERATE-ALL del libro
 * "Artificial Intelligence: A Modern Approach").
 */
class RedBayesiana {

    private:
        // Mapa de todas las variables de la red, indexadas por su nombre
        map<string, VariableAleatoria*> variablesAleatorias;

    public:
        // Constructor y destructor
        RedBayesiana();
        ~RedBayesiana();

        // Getters y Setters
        // Retorna el mapa completo de variables (util para consultas externas)
        map<string, VariableAleatoria*> getVariablesAleatorias();

        // Operaciones propias

        // Agrega una variable a la red si aun no existe
        void agregarVariableAleatoria(string nombre);
        // Crea el enlace padre -> hijo entre dos variables de la red
        void agregarConexiones(string padre, string hijo);
        // Lee y carga las CPTs desde un archivo de texto con formato definido
        void cargarTablaProbabilidadConjunta(string ruta);
        // Muestra en consola cada variable con sus padres
        void mostrarVariables();
        // Muestra en consola la CPT de cada variable
        void mostrarTablaProbabilidadConjunta();
        // Consulta P(variable = valor | evidencia) usando la CPT directamente
        double obtenerProbabilidad(VariableAleatoria* variable, string valor, map<string,string>& evidencia);
        // Funcion recursiva auxiliar del algoritmo de enumeracion
        double enumerarTodas(vector<VariableAleatoria*> variables, map<string,string>& evidencia, int profundidad = 0);
        // Calcula la distribucion de probabilidad de una variable dada una evidencia
        map<string,double> consultaPorEnumeracion(string variableConsulta, map<string,string>& evidencia);
        // Ordena las variables de la red segun el orden topologico del DAG
        vector<VariableAleatoria*> obtenerOrdenTopologico();
        // Funcion auxiliar DFS para el ordenamiento topologico
        void dfsTopologico(VariableAleatoria* variable, map<string,bool>& variablesVisitadas, vector<VariableAleatoria*>& orden);
};

#endif // REDBAYESIANA_H
