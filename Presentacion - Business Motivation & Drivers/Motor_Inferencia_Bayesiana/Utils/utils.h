/*********************************************************************************
-Pontificia Universidad Javeriana
-Proyecto 3 - Introduccion a la Inteligencia Artificial
-Temas: TADs, Compilacion Modular, Redes Bayesianas
*********************************************************************************/
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <map>

using namespace std;

// Obtiene los datos necesarios para realizar las consultas
bool parsearConsulta(const string& consulta, string& variableConsulta, string& valorConsulta, map<string,string>& evidencia);

#endif