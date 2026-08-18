/*********************************************************************************
-Pontificia Universidad Javeriana
-Proyecto 3 - Introduccion a la Inteligencia Artificial
-Temas: TADs, Compilacion Modular, Redes Bayesianas
*********************************************************************************/
#include "VariableAleatoria.h"

using namespace std;

/**
 * @brief Inicializa la variable con su nombre.
 *        Los vectores y la CPT quedan vacios hasta que se cargue el archivo de probabilidades.
 */
VariableAleatoria::VariableAleatoria(string nombre) {
    this->nombre = nombre;
}

/** @brief Destructor por defecto. */
VariableAleatoria::~VariableAleatoria() {}

// ──────────────────────────── Getters y Setters ────────────────────────────

/** @brief Retorna el nombre identificador de la variable. */
string VariableAleatoria::getNombre() {
    return nombre;
}

/** @brief Asigna un nuevo nombre a la variable. */
void VariableAleatoria::setNombre(string nombre) {
    this->nombre = nombre;
}

/** @brief Retorna la lista de punteros a las variables padre. */
vector<VariableAleatoria*> VariableAleatoria::getPadres() {
    return padres;
}

/** @brief Retorna la lista de punteros a las variables hijo. */
vector<VariableAleatoria*> VariableAleatoria::getHijos() {
    return hijos;
}

/**
 * @brief Retorna la lista de valores discretos posibles de esta variable.
 *        Se usa en enumerarTodas para marginalizar sobre todos los valores.
 */
vector<string> VariableAleatoria::getValores() {
    return valores;
}

/**
 * @brief Retorna referencia a la CPT completa.
 *        Se retorna por referencia para permitir consultas directas desde RedBayesiana.
 */
map<string, map<string, double>>& VariableAleatoria::getTablaProbabilidadCondicional() {
    return tablaProbabilidadCondicional;
}

/** @brief Reemplaza toda la CPT por una nueva. */
void VariableAleatoria::setTablaProbabilidadCondicional(map<string, map<string, double>> tabla) {
    tablaProbabilidadCondicional = tabla;
}

// ──────────────────────────── Operaciones propias ──────────────────────────

/** @brief Agrega un puntero a la lista de padres de esta variable. */
void VariableAleatoria::agregarPadre(VariableAleatoria* padre) {
    padres.push_back(padre);
}

/** @brief Agrega un puntero a la lista de hijos de esta variable. */
void VariableAleatoria::agregarHijo(VariableAleatoria* hijo) {
    hijos.push_back(hijo);
}

/**
 * @brief Registra un valor posible si aun no estaba en la lista.
 *        Evita duplicados para mantener la lista limpia.
 *
 * @param valor Valor discreto a registrar (ej: "none", "light", "heavy")
 */
void VariableAleatoria::agregarValor(string valor) {
    for (auto& v : valores) {
        if (v == valor) return;
    }
    valores.push_back(valor);
}

/**
 * @brief Inserta P(variable = valor | condicion) en la CPT.
 *
 * @param condicion   Condicion normalizada de padres, ej. "RAIN=none,MAINT=yes"
 *                    Para variables sin padres usar "" (string vacio)
 * @param valor       Valor de esta variable, ej. "on_time"
 * @param probabilidad P(variable = valor | condicion)
 */
void VariableAleatoria::agregarEntradaTabla(string condicion, string valor, double probabilidad) {
    tablaProbabilidadCondicional[condicion][valor] = probabilidad;
}