# Taller de introducción a Apache Spark

**Autor:** Gabriel Jaramillo Cuberos  
**Materia:** Procesamiento de Datos

Taller práctico de análisis y clasificación del conjunto de datos
Iris con PySpark, ejecutado desde Jupyter en una máquina virtual
conectada a un clúster Spark.

## Nota
El taller no contiene la dirección IP usada originalmente por cuestiones de seguridad.

## Archivos

| Archivo | Descripción |
|---|---|
| `Jaramillo_Gabriel_Tutorial_Apache_Spark.ipynb` | Cuaderno con el desarrollo, las verificaciones del entorno y los resultados. |
| `iris.csv` | Dataset con 150 registros, cuatro medidas de flores y su especie. |

## Actividades realizadas

- Configuración de Jupyter y conexión a Spark.
- Lectura y exploración del CSV.
- Conversión de especies a etiquetas numéricas.
- Construcción de vectores de características y escalado.
- División aleatoria con proporciones objetivo de 90 % para entrenamiento y 10 % para prueba.
- Entrenamiento y evaluación de Decision Tree, Random Forest y Naive Bayes.

## Entorno utilizado

- Linux y JupyterLab.
- Python 3.11.
- Apache Spark y PySpark 4.2.0.
- NumPy, pandas, SciPy y scikit-learn.
- Acceso SSH cuando se utiliza un navegador externo a la MV.

Las librerías deben instalarse en el entorno asociado al kernel
seleccionado. Las operaciones Python distribuidas también requieren
Python compatible y las dependencias necesarias en los workers.

## Ejecución

1. Abrir el cuaderno en Jupyter.
2. Seleccionar el kernel de Python 3.11 con las dependencias instaladas.
3. Adaptar la dirección del master, la ruta de Python y la ubicación
   de `fairscheduler.xml` a la infraestructura disponible.
4. Ajustar la ruta de `iris.csv`. Si se utiliza una ruta local,
   el archivo debe estar disponible en esa misma ruta en el driver
   y los workers que puedan leerlo.
5. Ejecutar las celdas en orden desde un kernel reiniciado.

> El cuaderno incluye celdas de instalación con `!pip install`.
> Para controlar el destino de las instalaciones, utilizar
> `python -m pip` con el ejecutable del entorno correspondiente,
> o `%pip` dentro del kernel. Conservar la versión de PySpark
> compatible con el clúster.

## Resultados registrados

| Modelo | Exactitud |
|---|---:|
| Decision Tree | 90.91 % |
| Random Forest | 100.00 % |
| Naive Bayes | 100.00 % |

Estos resultados corresponden a la ejecución guardada en el
cuaderno. No demuestran que un modelo sea siempre superior:
la evaluación utiliza una única partición y pocos datos de prueba.