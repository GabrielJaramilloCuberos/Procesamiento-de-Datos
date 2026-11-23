# Talleres de clasificación con PySpark y MLlib

**Autor** Gabriel Jaramillo Cuberos  
**Asignatura** Procesamiento de Datos (1255)  
**Universidad** Pontificia Universidad Javeriana

Dos talleres de clasificación binaria con Apache Spark. Cada cuaderno documenta la exploración de los datos, la preparación de variables, el entrenamiento de cinco clasificadores y la interpretación de sus resultados.

## Cuadernos

| Taller | Pregunta | Cuaderno |
|---|---|---|
| **Stroke (ICTUS)** | ¿Qué tan bien pueden los modelos identificar los registros etiquetados con derrame a partir de las características disponibles? | [Abrir cuaderno de Stroke](Stroke/1255JaramilloStroke.ipynb) |
| **Salarios (Census Income)** | ¿Qué tan bien pueden los modelos distinguir los registros con ingresos anuales superiores a 50.000 dólares? | [Abrir cuaderno de Salarios](Salarios/JaramilloSalario_1255.ipynb) |

## Stroke (ICTUS)

El taller trabaja con **5.110 registros** en el bloque final de modelado. Se exploran atributos como edad, nivel de glucosa, IMC y antecedentes de salud. La variable objetivo es `Derrame`, donde `1` identifica los registros etiquetados con derrame.

La preparación conserva la edad decimal, codifica las variables categóricas y añade un indicador de IMC faltante. Los valores ausentes de IMC se completan con la mediana aprendida en entrenamiento. La separación estratificada deja **4.087 registros para entrenamiento** y **1.023 para prueba**, con 50 positivos en el conjunto de prueba.

**Resultado destacado.** Cuatro modelos obtuvieron **95,11 % de exactitud** al clasificar todos los registros como negativos. El perceptrón multicapa fue el único que identificó positivos con las reglas de decisión utilizadas. Detectó **6 de 50**, con **12,00 % de sensibilidad** y **F1 de 0,1690**. El bosque aleatorio logró la mayor **PR-AUC (0,2296)**, aunque su sensibilidad fue cero con el umbral utilizado. Esa diferencia permite distinguir entre ordenar registros por puntuación y clasificarlos como positivos.

El alcance del ejercicio es la clasificación de la etiqueta histórica del archivo. Cualquier aplicación clínica requeriría validación externa.

## Salarios (Census Income)

El taller utiliza **48.842 registros** de [Census Income (Adult) de UCI](https://archive.ics.uci.edu/dataset/20/census+income), derivados de datos censales de 1994. La clase positiva representa ingresos anuales `>50K`.

Los archivos originales se leen por separado. `adult.data` se divide en **25.913 registros de entrenamiento** y **6.648 de validación**. Los **16.281 registros de `adult.test`** se reservan para la prueba oficial. La carga asigna los encabezados, trata `?` como dato faltante y elimina el punto final de las etiquetas de prueba. La preparación reutiliza funciones de `Utils/proyecto.py` para limpiar y auditar los datos.

**Resultado destacado.** El perceptrón multicapa se seleccionó por su **F1 de 0,6891 en validación**. En la prueba oficial obtuvo **85,96 % de exactitud**, **62,30 % de sensibilidad**, **74,13 % de precisión positiva** y **F1 de 0,6770**. Identificó **2.396 de los 3.846 positivos**. La referencia que siempre predice `<=50K` alcanzó **76,38 % de exactitud** y **0 % de sensibilidad**.

## Modelos y evaluación

Ambos talleres implementan cinco clasificadores de `pyspark.ml`:

1. Regresión logística.
2. Árbol de decisión.
3. Bosque aleatorio.
4. SVM lineal.
5. Perceptrón multicapa.

Los cuadernos ajustan la codificación y el escalado con datos de entrenamiento. La evaluación incluye exactitud, precisión, sensibilidad y F1 para la clase positiva, además de ROC-AUC, PR-AUC y matrices de confusión. Cada conjunto de métricas se interpreta dentro de su propio taller, según la proporción de positivos y los errores observados.