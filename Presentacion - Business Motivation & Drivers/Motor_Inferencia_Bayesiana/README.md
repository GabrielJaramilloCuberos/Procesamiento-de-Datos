# Motor de Inferencia — Red Bayesiana 
Pontificia Universidad Javeriana

---

## Descripción

Este motor implementa una **Red de Inferencia Bayesiana** en C++. Una Red Bayesiana es un grafo acíclico dirigido (DAG) donde cada nodo representa una variable aleatoria y las aristas representan dependencias probabilísticas entre ellas. Junto con un script de Python, genera los datos necesarios para construir la red y poder explicar los **motores o impulsores de negocio** en procesamiento de datos.

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

## Compilación, ejecución y destrucción

### Compilar y ejecutar
```bash
make run
```

### Destruir
```bash
make clean
```
