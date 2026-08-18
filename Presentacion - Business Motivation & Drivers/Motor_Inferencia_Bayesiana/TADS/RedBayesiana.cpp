/*********************************************************************************
-Pontificia Universidad Javeriana
-Proyecto 3 - Introduccion a la Inteligencia Artificial
-Temas: TADs, Compilacion Modular, Redes Bayesianas
*********************************************************************************/
#include "RedBayesiana.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

/** @brief Constructor por defecto. La red inicia sin variables. */
RedBayesiana::RedBayesiana() {}

/**
 * @brief Destructor: libera la memoria de todas las variables creadas con new.
 *        Es suficiente eliminar los valores del mapa; las referencias cruzadas
 *        en padres/hijos quedan invalidas pero la red deja de existir de todos modos.
 */
RedBayesiana::~RedBayesiana() {
    for (auto& par : variablesAleatorias) {
        delete par.second;
    }
}

// ──────────────────────────── Getters y Setters ────────────────────────────

/** @brief Retorna el mapa completo de variables para consultas externas. */
map<string, VariableAleatoria*> RedBayesiana::getVariablesAleatorias() {
    return variablesAleatorias;
}

// ──────────────────────────── Operaciones propias ──────────────────────────

/**
 * @brief Agrega una nueva variable a la red solo si su nombre no existe aun.
 *        Esto evita duplicados al procesar el archivo de variables.
 *
 * @param nombre Identificador unico de la variable
 */
void RedBayesiana::agregarVariableAleatoria(string nombre) {
    if (variablesAleatorias.find(nombre) == variablesAleatorias.end()) {
        variablesAleatorias[nombre] = new VariableAleatoria(nombre);
    }
}

/**
 * @brief Establece la relacion direccional padre -> hijo entre dos variables.
 *        Si alguna de las dos variables no existe, la crea antes de conectarlas.
 *
 * @param padre Nombre de la variable que influye (nodo origen)
 * @param hijo  Nombre de la variable influenciada (nodo destino)
 */
void RedBayesiana::agregarConexiones(string padre, string hijo) {
    agregarVariableAleatoria(padre);
    agregarVariableAleatoria(hijo);

    VariableAleatoria* variablePadre = variablesAleatorias[padre];
    VariableAleatoria* variableHijo  = variablesAleatorias[hijo];

    variablePadre->agregarHijo(variableHijo);
    variableHijo->agregarPadre(variablePadre);
}

/**
 * @brief Carga las tablas de probabilidad condicional (CPTs) desde un archivo.
 *
 * El archivo se procesa linea por linea. Se reconocen tres tipos de lineas:
 *  1. Solo un token  -> nombre de variable sin padres (probabilidad incondicional)
 *  2. Linea con '|'  -> encabezado de variable con padres (probabilidad condicional)
 *  3. Linea de datos -> "valor probabilidad" o "condicion valor probabilidad"
 *
 * Solo se almacena P(variable = true). P(false) = 1 - P(true).
 *
 * @param ruta Ruta del archivo .txt con las probabilidades
 */
void RedBayesiana::cargarTablaProbabilidadConjunta(string ruta) {
    cout << "Cargando probabilidades..." << endl << endl;
    ifstream archivo(ruta);
    string linea;

    // Apunta a la variable cuya CPT se esta llenando en cada momento
    VariableAleatoria* variableActual = nullptr;

    if (archivo.is_open()) {
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            stringstream ss(linea);
            string a, b;
            ss >> a >> b;

            // Caso 1: linea con un solo token -> encabezado de variable sin padres
            if (b.empty()) {
                if (variablesAleatorias.find(a) == variablesAleatorias.end()) {
                    continue;
                }
                variableActual = variablesAleatorias[a];
                continue;
            }

            // Caso 2: linea con '|' -> encabezado de variable con padres
            if (linea.find('|') != string::npos) {
                if (variablesAleatorias.find(a) == variablesAleatorias.end()) {
                    cout << "ERROR: Variable no encontrada: " << a << endl;
                    continue;
                }
                variableActual = variablesAleatorias[a];
                continue;
            }

            // Caso 3: linea de datos -> tokenizar y cargar la probabilidad
            stringstream lectura(linea);
            vector<string> tokens;
            string temporal;
            while (lectura >> temporal) {
                tokens.push_back(temporal);
            }

            if (variableActual == nullptr) {
                cout << "variableActual es null" << endl;
                continue;
            }

            // Formato "true/false probabilidad" (variable sin padres, clave vacia)
            // Formato "valor probabilidad" (variable sin padres)
            if (tokens.size() == 2) {
                string valor       = tokens[0];
                double probabilidad = stod(tokens[1]);
                // Registrar el valor como posible y guardarlo en la CPT con condicion vacia
                variableActual->agregarValor(valor);
                variableActual->agregarEntradaTabla("", valor, probabilidad);
            }
            // Formato "condicion valor probabilidad" (variable con padres)
            else if (tokens.size() == 3) {
                string condicion    = tokens[0];
                string valor        = tokens[1];
                double probabilidad = stod(tokens[2]);

                // Normalizar condicion ordenando sus componentes alfabeticamente
                vector<string> partes;
                string parte;
                stringstream ssCondicion(condicion);
                while (getline(ssCondicion, parte, ',')) partes.push_back(parte);
                sort(partes.begin(), partes.end());

                string condicionNorm = "";
                for (int i = 0; i < partes.size(); i++) {
                    if (i > 0) condicionNorm += ",";
                    condicionNorm += partes[i];
                }

                // Registrar el valor como posible y guardarlo en la CPT
                variableActual->agregarValor(valor);
                variableActual->agregarEntradaTabla(condicionNorm, valor, probabilidad);
            }
        }
        cout << "Datos cargados" << endl;
        archivo.close();
    } else {
        cerr << "Error al abrir archivo." << endl;
    }
}

/**
 * @brief Imprime en consola cada variable con la lista de sus padres directos.
 *        Util para verificar que la estructura del DAG se cargo correctamente.
 */
void RedBayesiana::mostrarVariables() {
    for (auto& par : variablesAleatorias) {
        cout << "Variable: " << par.first << endl;
        cout << "Padres: ";
        for (auto& padre : par.second->getPadres()) {
            cout << padre->getNombre() << " ";
        }
        cout << endl << endl;
    }
}

/**
 * @brief Imprime en consola la tabla de probabilidad condicional de cada variable.
 *        Cada entrada muestra la condicion y su P(variable = true | condicion).
 */
void RedBayesiana::mostrarTablaProbabilidadConjunta() {
    for (auto& par : variablesAleatorias) {
        cout << "Variable: " << par.first << endl;
        // Iterar sobre condiciones y luego sobre cada valor -> probabilidad
        for (auto& entrada : par.second->getTablaProbabilidadCondicional()) {
            string condicion = entrada.first.empty() ? "(sin padres)" : entrada.first;
            for (auto& valorProb : entrada.second) {
                cout << "  " << condicion << " | " << valorProb.first
                     << " -> " << valorProb.second << endl;
            }
        }
        cout << endl;
    }
}

/**
 * @brief Consulta P(variable = valor | evidencia) directamente desde la CPT.
 *
 * Construye la clave de busqueda recorriendo los padres de la variable
 * y leyendo su valor actual en el mapa de evidencia. Si el valor buscado
 * es false, retorna 1 - P(true) por complemento.
 *
 * @param variable Puntero a la variable cuya probabilidad se busca
 * @param valor    Valor de la variable (true o false)
 * @param evidencia Mapa con los valores conocidos de otras variables
 * @return P(variable = valor | evidencia segun la CPT)
 */
double RedBayesiana::obtenerProbabilidad(VariableAleatoria* variable, string valor, map<string,string>& evidencia) {
    // Construir clave normalizada (ordenada) a partir de los padres
    vector<pair<string,string>> componentes;
    for (auto padre : variable->getPadres()) {
        if (evidencia.find(padre->getNombre()) == evidencia.end()) {
            cout << "Error: evidencia faltante para " << padre->getNombre() << endl;
            return 0;
        }
        componentes.push_back({padre->getNombre(), evidencia.at(padre->getNombre())});
    }
    sort(componentes.begin(), componentes.end());

    string clave = "";
    for (int i = 0; i < componentes.size(); i++) {
        if (i > 0) clave += ",";
        clave += componentes[i].first + "=" + componentes[i].second;
    }

    auto& tabla = variable->getTablaProbabilidadCondicional();
    if (tabla.find(clave) == tabla.end() || tabla.at(clave).find(valor) == tabla.at(clave).end()) {
        cout << "Error: no se encontro P(" << variable->getNombre()
             << "=" << valor << " | " << clave << ")" << endl;
        return 0;
    }

    // Ya no se necesita el complemento: todos los valores estan almacenados explicitamente
    return tabla.at(clave).at(valor);
}

/**
 * @brief Implementacion del algoritmo ENUMERATE-ALL.
 *
 * Recorre recursivamente todas las variables en orden topologico.
 * Si la variable ya tiene un valor en la evidencia, usa ese valor directamente.
 * Si no, suma las dos ramas posibles (true y false) y marginaliza la variable.
 *
 * @param variables Lista de variables aun por procesar (en orden topologico)
 * @param evidencia Mapa de asignaciones conocidas (se modifica temporalmente)
 * @return Probabilidad conjunta acumulada hasta este punto de la recursion
 */
double RedBayesiana::enumerarTodas(vector<VariableAleatoria*> variables, map<string,string>& evidencia, int profundidad) {
    string ind(profundidad * 2, ' ');

    if (variables.empty()) {
        cout << ind << "-> prob. parcial = 1.0" << endl;
        return 1.0;
    }

    VariableAleatoria* varTem = variables[0];
    vector<VariableAleatoria*> resto(variables.begin() + 1, variables.end());

    if (evidencia.find(varTem->getNombre()) != evidencia.end()) {
        // Variable con valor conocido: usar directamente
        string valorTem = evidencia[varTem->getNombre()];
        double p = obtenerProbabilidad(varTem, valorTem, evidencia);
        cout << ind << "[E] " << varTem->getNombre() << "=" << valorTem << " | P=" << p << endl;
        return p * enumerarTodas(resto, evidencia, profundidad + 1);
    } else {
        // Variable desconocida: marginalizar sumando sobre TODOS sus valores posibles
        cout << ind << "[M] " << varTem->getNombre() << " (marginalizando)" << endl;
        double suma = 0;

        for (string val : varTem->getValores()) {
            evidencia[varTem->getNombre()] = val;
            double pVal = obtenerProbabilidad(varTem, val, evidencia);
            cout << ind << "  rama " << val << " | P=" << pVal << endl;
            suma += pVal * enumerarTodas(resto, evidencia, profundidad + 1);
        }

        evidencia.erase(varTem->getNombre());
        cout << ind << "  suma marginalizada = " << suma << endl;
        return suma;
    }
}

/**
 * @brief Calcula la distribucion de probabilidad P(X | evidencia) usando
 *        inferencia exacta por enumeracion.
 *
 * Ejecuta ENUMERATE-ALL dos veces (una para X=true, otra para X=false)
 * y normaliza los resultados con el factor alfa = 1 / (P(true) + P(false)).
 *
 * @param variableConsulta Nombre de la variable X que se quiere inferir
 * @param evidencia        Mapa de variables observadas con su valor conocido
 * @return Mapa {true -> P(X=true|e), false -> P(X=false|e)} normalizado
 */
map<string,double> RedBayesiana::consultaPorEnumeracion(string variableConsulta, map<string,string>& evidencia) {
    map<string,double> distribucion;
    vector<VariableAleatoria*> variables = obtenerOrdenTopologico();
    VariableAleatoria* varX = variablesAleatorias[variableConsulta];

    cout << "\n========== TRAZA DE INFERENCIA ==========" << endl;
    cout << "Consulta  : " << variableConsulta << endl;
    cout << "Evidencia : ";
    for (auto& par : evidencia) cout << par.first << "=" << par.second << " ";
    cout << endl << "Orden topologico: ";
    for (auto v : variables) cout << v->getNombre() << " ";
    cout << "\n" << endl;

    double total = 0;
    // Iterar sobre todos los valores posibles de la variable consulta
    for (string val : varX->getValores()) {
        cout << "-- Enumeracion " << variableConsulta << " = " << val << " --" << endl;
        evidencia[variableConsulta] = val;
        double prob = enumerarTodas(variables, evidencia, 0);
        distribucion[val] = prob;
        total += prob;
        cout << "   Resultado sin normalizar: " << prob << "\n" << endl;
    }

    evidencia.erase(variableConsulta);

    // Normalizar con alfa
    cout << "-- Normalizacion (alfa = 1/" << total << " = " << 1.0/total << ") --" << endl;
    for (auto& par : distribucion) {
        par.second /= total;
        cout << "   P(" << variableConsulta << "=" << par.first << " | evidencia) = " << par.second << endl;
    }
    cout << "=========================================" << endl << endl;

    return distribucion;
}

/**
 * @brief Funcion auxiliar DFS para el ordenamiento topologico.
 *
 * Visita en profundidad todos los hijos de la variable actual antes
 * de agregarla al vector de orden. Al invertir el resultado en
 * obtenerOrdenTopologico() se obtiene el orden correcto (padres antes que hijos).
 *
 * @param variable           Variable actual en la recursion
 * @param variablesVisitadas Mapa que indica si una variable ya fue procesada
 * @param orden              Vector donde se acumula el orden de visita
 */
void RedBayesiana::dfsTopologico(VariableAleatoria* variable, map<string,bool>& variablesVisitadas, vector<VariableAleatoria*>& orden) {
    variablesVisitadas[variable->getNombre()] = true;

    // Visitar primero todos los hijos no visitados
    for (auto hijo : variable->getHijos()) {
        if (!variablesVisitadas[hijo->getNombre()]) {
            dfsTopologico(hijo, variablesVisitadas, orden);
        }
    }

    // Agregar esta variable despues de haber procesado sus descendientes
    orden.push_back(variable);
}

/**
 * @brief Retorna las variables de la red en orden topologico (padres antes que hijos).
 *
 * Usa DFS post-orden sobre el DAG y luego invierte el resultado.
 * Este orden es necesario para que ENUMERATE-ALL siempre tenga disponibles
 * los valores de los padres antes de evaluar a sus hijos.
 *
 * @return Vector de punteros a VariableAleatoria en orden topologico
 */
vector<VariableAleatoria*> RedBayesiana::obtenerOrdenTopologico() {
    vector<VariableAleatoria*> orden;
    map<string,bool> variablesVisitadas;

    // Inicializar todas las variables como no visitadas
    for (auto& par : variablesAleatorias) {
        variablesVisitadas[par.first] = false;
    }

    // Ejecutar DFS desde cada nodo no visitado (maneja grafos desconectados)
    for (auto& par : variablesAleatorias) {
        if (!variablesVisitadas[par.first]) {
            dfsTopologico(par.second, variablesVisitadas, orden);
        }
    }

    // DFS produce post-orden; invertir da el orden topologico correcto
    reverse(orden.begin(), orden.end());

    return orden;
}
