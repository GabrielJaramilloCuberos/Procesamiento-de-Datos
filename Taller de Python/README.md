# Taller de Introducción a Python 🐍

Material de trabajo de la asignatura **Procesamiento de Datos (1255)**.  
El taller presenta una introducción progresiva a Python mediante nueve módulos y un práctico integrador.

## Contenido 📃

El material está organizado de la siguiente manera:

| # | Archivo | Tema |
|---|---|---|
| 1 | `01_Python_Cadenas.ipynb` | Cadenas |
| 2 | `02_Python_Tuplas.ipynb` | Tuplas |
| 3 | `03_Python_Listas.ipynb` | Listas |
| 4 | `04_Python_Conjuntos.ipynb` | Conjuntos |
| 5 | `05_Python_Diccionarios.ipynb` | Diccionarios |
| 6 | `06_Python_Condiciones.ipynb` | Condiciones |
| 7 | `07_Python_Bucles.ipynb` | Bucles |
| 8 | `08_Python_Funciones.ipynb` | Funciones |
| 9 | `09_Python_Clases.ipynb` | Clases y objetos |
| 10 | `Practico_Bono_1.ipynb` | Ejercicios integradores |

## Objetivo 🎯

Desarrollar los fundamentos necesarios para programar en Python, comenzando por el manejo de datos y avanzando hacia estructuras de control, funciones y programación orientada a objetos.

## Módulos 〽️

### 1. Cadenas

Se trabajan:

- Cadenas de caracteres.
- Indexación.
- Indexación negativa.
- Slicing.
- Stride.
- Concatenación.
- Secuencias de escape.
- Operaciones con cadenas.

### 2. Tuplas

Se estudian:

- Creación y manejo de tuplas.
- Indexación.
- Concatenación.
- Slicing.
- Tuplas anidadas.
- Ordenamiento.

### 3. Listas

Se abordan:

- Indexación.
- Slicing.
- Listas con diferentes tipos de datos.
- Listas anidadas.
- Modificación de elementos.
- Operaciones con listas.
- Copiado y clonación.

### 4. Conjuntos

Se estudian las operaciones fundamentales sobre conjuntos:

- Unión.
- Intersección.
- Diferencia.
- Diferencia simétrica.
- Operaciones lógicas entre conjuntos.

### 5. Diccionarios

Se introduce el modelo de datos basado en **llave → valor**:

```python
persona = {
    "nombre": "Gabriel",
    "edad": 21
}

print(persona["nombre"])
```

### 6. Condiciones

Se trabajan:

- Operadores de comparación.
- `if`.
- `else`.
- `elif`.
- Operadores lógicos.
- Construcción de decisiones según diferentes condiciones.

Ejemplo:

```python
edad = 20

if edad > 18:
    print("Puede ingresar")
else:
    print("No puede ingresar")
```

### 7. Bucles

Se estudian:

- `range`.
- `for`.
- `while`.

Ejemplo:

```python
for numero in range(5):
    print(numero)
```

### 8. Funciones

Se introducen:

- Funciones predefinidas.
- Funciones definidas por el usuario.
- Parámetros y argumentos.
- `return`.
- Valores predeterminados.
- Condiciones y ciclos dentro de funciones.
- Variables globales.
- Variables locales.
- Alcance (`scope`).

Ejemplo:

```python
def sumar(a, b):
    return a + b

resultado = sumar(4, 6)
print(resultado)
```

### 9. Clases y objetos

Se presentan los fundamentos de programación orientada a objetos:

- Clases.
- Objetos o instancias.
- Atributos.
- Métodos.
- Constructor `__init__`.
- Uso de `self`.

Ejemplo:

```python
class Circle:
    def __init__(self, radius=3, color="blue"):
        self.radius = radius
        self.color = color

    def add_radius(self, r):
        self.radius += r

circulo = Circle(10, "red")
circulo.add_radius(2)

print(circulo.radius)
```

El módulo también plantea la creación de una clase `Elipse` utilizando `matplotlib`, con métodos para cambiar el ancho, alto, color de relleno, color de borde y dibujar la figura.

## Práctico Bono

`Practico_Bono_1.ipynb` reúne ejercicios para aplicar los conceptos de los nueve módulos anteriores.

### Calentamiento

- **Menor de dos pares**
- **Galletas de animales**
- **Hace veinte**

### Nivel 1

- **Mayúsculas**
- **Reversa**

### Nivel 2

- **Problema 33**
- **Replicador**
- **Blackjack**

Estos ejercicios combinan principalmente funciones, condiciones, cadenas, listas y bucles.

## Requisitos

Se recomienda trabajar con:

- Python 3.
- Jupyter Notebook o JupyterLab.
- Un entorno que permita ejecutar archivos `.ipynb`.

Para instalar Jupyter Notebook mediante `pip`:

```bash
pip install notebook
```

Para iniciar Jupyter:

```bash
jupyter notebook
```

También puede utilizarse JupyterLab:

```bash
pip install jupyterlab
jupyter lab
```

## Orden recomendado

Se recomienda seguir los cuadernos en este orden:

```text
01 Cadenas
   ↓
02 Tuplas
   ↓
03 Listas
   ↓
04 Conjuntos
   ↓
05 Diccionarios
   ↓
06 Condiciones
   ↓
07 Bucles
   ↓
08 Funciones
   ↓
09 Clases
   ↓
Práctico Bono
```

El orden permite avanzar desde el manejo básico de datos hacia la construcción de programas con mayor nivel de abstracción.

## Estructura del repositorio

Una organización sugerida es:

```text
.
├── 01_Python_Cadenas.ipynb
├── 02_Python_Tuplas.ipynb
├── 03_Python_Listas.ipynb
├── 04_Python_Conjuntos.ipynb
├── 05_Python_Diccionarios.ipynb
├── 06_Python_Condiciones.ipynb
├── 07_Python_Bucles.ipynb
├── 08_Python_Funciones.ipynb
├── 09_Python_Clases.ipynb
├── Practico_Bono_1.ipynb
├── informe_taller_python.tex
└── README.md
```

## Autor

**Gabriel Jaramillo Cuberos**  
Procesamiento de Datos (1255)
