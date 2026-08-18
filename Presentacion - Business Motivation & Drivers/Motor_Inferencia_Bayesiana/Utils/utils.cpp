/*********************************************************************************
-Pontificia Universidad Javeriana
-Proyecto 3 - Introduccion a la Inteligencia Artificial
-Temas: TADs, Compilacion Modular, Redes Bayesianas
*********************************************************************************/
#include "utils.h"
#include <sstream>

using namespace std;

/**
 * @brief Tokeniza las consultas solicitadas por el usuario.
 *
 * @param consulta Consulta enviada por el usuario
 * @param variableConsulta Almacena las variable que solicita el usuario
 * @param evidencia Almacena la evidencia que adjunta el usuario
 */
bool parsearConsulta(const string& consulta, string& variableConsulta, string& valorConsulta, map<string,string>& evidencia) {
    size_t inicio = consulta.find('(');
    size_t fin    = consulta.rfind(')');
    if (inicio == string::npos || fin == string::npos) return false;

    string contenido = consulta.substr(inicio + 1, fin - inicio - 1);

    size_t separador = contenido.find('|');
    if (separador == string::npos) {
        string item;
        stringstream(contenido) >> item;
        size_t eq = item.find('=');
        if (eq != string::npos) {
            variableConsulta = item.substr(0, eq);
            valorConsulta    = item.substr(eq + 1);
        } else {
            variableConsulta = item;
            valorConsulta    = "";
        }
        return true;
    }

    string izquierda = contenido.substr(0, separador);
    string derecha   = contenido.substr(separador + 1);

    string token;
    stringstream(izquierda) >> token;
    size_t eq = token.find('=');
    if (eq != string::npos) {
        variableConsulta = token.substr(0, eq);
        valorConsulta    = token.substr(eq + 1); // capturar el valor si viene especificado
    } else {
        variableConsulta = token;
        valorConsulta    = "";
    }

    string par;
    stringstream ssDerecha(derecha);
    while (getline(ssDerecha, par, ',')) {
        string item;
        stringstream(par) >> item;
        size_t pos = item.find('=');
        if (pos == string::npos) return false;
        // El valor ya no se convierte a bool, se guarda como string directamente
        evidencia[item.substr(0, pos)] = item.substr(pos + 1);
    }

    return true;
}