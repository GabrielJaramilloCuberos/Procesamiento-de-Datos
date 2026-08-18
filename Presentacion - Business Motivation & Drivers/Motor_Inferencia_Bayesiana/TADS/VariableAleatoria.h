/*********************************************************************************
-Pontificia Universidad Javeriana
-Proyecto 3 - Introduccion a la Inteligencia Artificial
-Temas: TADs, Compilacion Modular, Redes Bayesianas
*********************************************************************************/
#ifndef VARIABLEALEATORIA_H
#define VARIABLEALEATORIA_H

#include <string>
#include <vector>
#include <map>

using namespace std;

/**
 * @brief Representa un nodo dentro de una Red Bayesiana.
 *
 * Soporta variables con cualquier cantidad de valores discretos (no solo true/false).
 * La CPT mapea una condicion de padres a la distribucion de probabilidad
 * sobre todos los valores posibles de esta variable.
 */
class VariableAleatoria {

    private:
        // Nombre unico que identifica la variable en la red
        string nombre;
        // Punteros a las variables de las que esta variable depende directamente
        vector<VariableAleatoria*> padres;
        // Punteros a las variables que dependen directamente de esta variable
        vector<VariableAleatoria*> hijos;
        // Todos los valores discretos posibles de esta variable (ej: "none","light","heavy")
        // Se pobla automaticamente al cargar el archivo de probabilidades
        vector<string> valores;
        // Tabla de probabilidad condicional (CPT).
        // Clave externa: condicion normalizada de los padres, ej. "RAIN=none,MAINT=yes"
        //                Si la variable no tiene padres, la clave es "" (string vacio)
        // Clave interna: valor de esta variable, ej. "on_time"
        // Valor:         P(variable = valor | condicion)
        map<string, map<string, double>> tablaProbabilidadCondicional;

    public:
        // Constructor: inicializa la variable con su nombre
        VariableAleatoria(string nombre);

        // Destructor por defecto
        ~VariableAleatoria();

        // Getters y Setters
        string getNombre();
        void setNombre(string nombre);
        vector<VariableAleatoria*> getPadres();
        vector<VariableAleatoria*> getHijos();
        vector<string> getValores();
        map<string, map<string, double>>& getTablaProbabilidadCondicional();
        void setTablaProbabilidadCondicional(map<string, map<string, double>> tabla);

        // Operaciones propias

        // Registra una variable como padre de esta
        void agregarPadre(VariableAleatoria* padre);
        // Registra una variable como hijo de esta
        void agregarHijo(VariableAleatoria* hijo);
        // Registra un nuevo valor posible si aun no estaba en la lista
        void agregarValor(string valor);
        // Inserta P(variable = valor | condicion) en la CPT
        void agregarEntradaTabla(string condicion, string valor, double probabilidad);

};

#endif // VARIABLEALEATORIA_H