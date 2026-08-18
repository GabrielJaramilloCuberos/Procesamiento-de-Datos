/*********************************************************************************
-Pontificia Universidad Javeriana
-Proyecto 3 - Introduccion a la Inteligencia Artificial
-Temas: TADs, Compilacion Modular, Redes Bayesianas
*********************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include "TADS/RedBayesiana.h"
#include "Utils/utils.h"

using namespace std;

int main() {

    // Instancia de la red bayesiana que se construye durante la sesion
    RedBayesiana redBayesiana;
    string linea;
    string ruta;

    cout << "<-----> Bienvenido al motor de inferencia <-----> " << endl << endl;

    int opcion = -1;

    while (opcion != 0) {

        cout << "Menu:" << endl;
        cout << "1. Cargar variables\n2. Cargar probabilidades\n3. Obtener probabilidad local\n4. Consulta por enumeracion\n0. Salir\n";
        cout << "Ingrese la opcion: ";
        cin >> opcion;

        switch (opcion) {

        // ── Opcion 0: salir del programa ──────────────────────────────────
        case 0: {
            cout << "Hasta luego." << endl;
            exit(0);
        }

        // ── Opcion 1: cargar la estructura del DAG desde archivo ──────────
        // Lee un .txt donde cada linea define una relacion "PADRE HIJO".
        // Tambien calcula y muestra el orden topologico de la red resultante.
        case 1: {
            cout << endl << "Ingrese el nombre del archivo donde estan las variables: ";
            cin >> ruta;
            ruta += ".txt";

            ifstream variables(ruta);

            if (variables.is_open()) {
                while (getline(variables, linea)) {
                    if (linea.size() == 0) continue;
                    stringstream ss(linea);
                    string padre, hijo;
                    ss >> padre >> hijo;
                    redBayesiana.agregarConexiones(padre, hijo);
                }
                variables.close();
                cout << endl << "Variables cargadas." << endl << endl;
            } else {
                cerr << "Error al abrir archivo." << endl;
                break;
            }

            // Mostrar el orden topologico para verificar la estructura de la red
            vector<VariableAleatoria*> orden = redBayesiana.obtenerOrdenTopologico();
            cout << "Orden topologico:" << endl;
            for (auto variable : orden) {
                cout << variable->getNombre() << " ";
            }
            cout << endl << endl;

            break;
        }

        // ── Opcion 2: cargar las tablas de probabilidad condicional ───────
        // Requiere que la red ya tenga variables cargadas (opcion 1 primero).
        // Lee un .txt con el formato de CPTs y muestra la red resultante.
        case 2: {
            map<string,VariableAleatoria*> contenedor = redBayesiana.getVariablesAleatorias();

            if (contenedor.empty()) {
                cout << "Error: no hay variables registradas." << endl << endl;
                break;
            }

            cout << "Ingrese el nombre del archivo donde estan las probabilidades: ";
            cin >> ruta;
            ruta += ".txt";

            cout << endl << "Red creada." << endl << endl;

            redBayesiana.cargarTablaProbabilidadConjunta(ruta);
            redBayesiana.mostrarVariables();
            redBayesiana.mostrarTablaProbabilidadConjunta();

            break;
        }

        // ── Opcion 3: probabilidad local desde la CPT ─────────────────────
        // Calcula P(variable = valor | evidencia) consultando directamente
        // la tabla de probabilidad condicional de la variable indicada.
        // El usuario ingresa la variable, el valor y la evidencia necesaria.
        case 3: {
            cin.ignore();
            cout << "Formato: P(VARIABLE=valor | EVIDENCIA1=valor,...)" << endl;
            cout << "> ";
            string consulta;
            getline(cin, consulta);

            string variableConsulta, valorConsulta;
            map<string,string> evidencia;

            if (!parsearConsulta(consulta, variableConsulta, valorConsulta, evidencia)) {
                cout << "Error: formato invalido." << endl << endl;
                break;
            }
            if (valorConsulta.empty()) {
                cout << "Error: debe especificar el valor. Ej: P(RAIN=none | ...)" << endl << endl;
                break;
            }

            VariableAleatoria* variable = redBayesiana.getVariablesAleatorias()[variableConsulta];
            double resultado = redBayesiana.obtenerProbabilidad(variable, valorConsulta, evidencia);
            cout << "P(" << variableConsulta << "=" << valorConsulta << " | evidencia) = " << resultado << endl << endl;            
            break;
        }

        // ── Opcion 4: inferencia exacta por enumeracion ───────────────────
        // Calcula P(X | evidencia) para cualquier variable X de la red usando
        // el algoritmo ENUMERATE-ALL sobre todas las variables en orden topologico.
        // El resultado es la distribucion completa P(X=true) y P(X=false).
        case 4: {
            map<string,VariableAleatoria*> contenedor = redBayesiana.getVariablesAleatorias();

            if (contenedor.empty()) {
                cout << endl << "Error: no hay red bayesiana." << endl << endl;
                break;
            }

            cin.ignore();

            cout << endl << "Ingrese la consulta [Formato: P(VARIABLE | EVIDENCIA1=valor,EVIDENCIA2=valor, ...)]: ";
            string consulta;
            getline(cin, consulta);

            string variableConsulta, valorConsulta;
            map<string,string> evidencia;

            if (!parsearConsulta(consulta, variableConsulta, valorConsulta, evidencia)) {
                cout << "Error: formato de consulta invalido." << endl << endl;
                break;
            }

            if (contenedor.find(variableConsulta) == contenedor.end()) {
                cout << "Error: la variable '" << variableConsulta << "' no existe en la red." << endl << endl;
                break;
            }

            map<string,double> resultado = redBayesiana.consultaPorEnumeracion(variableConsulta, evidencia);

            cout << endl;
            for (auto& par : resultado) {
                cout << "P(" << variableConsulta << "=" << par.first << " | evidencia) = " << par.second << endl;
            }
            cout << endl;

            break;
        }

        default:
            cout << endl << "Opcion invalida." << endl << endl;
            break;
        }
    }

    return 0;
}