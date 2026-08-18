# Motor de Inferencia — Red Bayesiana
**Proyecto 3 · Introducción a la Inteligencia Artificial**  
Pontificia Universidad Javeriana

---

## Descripción

Este proyecto implementa un **motor de inferencia basado en Redes Bayesianas** en C++. Una Red Bayesiana es un grafo acíclico dirigido (DAG) donde cada nodo representa una variable aleatoria y las aristas representan dependencias probabilísticas entre ellas.

El programa permite:
- Cargar la estructura de la red (variables y sus relaciones padre-hijo) desde un archivo de texto.
- Cargar las tablas de probabilidad condicional (CPTs) de cada variable desde otro archivo.
- Visualizar la red cargada con sus dependencias y probabilidades.

---

## Estructura del proyecto

```
Motor_Inferencia_Bayesiana/
├── TADS/
│   ├── VariableAleatoria.h      # Declaracion del TAD VariableAleatoria
│   ├── VariableAleatoria.cpp    # Implementacion del TAD VariableAleatoria
│   ├── RedBayesiana.h           # Declaracion del TAD RedBayesiana
│   └── RedBayesiana.cpp         # Implementacion del TAD RedBayesiana
├── main.cpp                     # Punto de entrada y menu de usuario
├── variables.txt                # Archivo de ejemplo: estructura de la red
├── probabilidades.txt           # Archivo de ejemplo: tablas de probabilidad
└── README.md
```

---

## TADs implementados

### `VariableAleatoria`
Representa un nodo dentro de la red bayesiana.


### `RedBayesiana`
Representa la red completa y gestiona todas las variables y sus conexiones.

## Compilación y ejecución

### Compilar
```bash
g++ main.cpp TADS/VariableAleatoria.cpp TADS/RedBayesiana.cpp Utils/utils.cpp -o motor
```

### Ejecutar
```bash
./motor
```