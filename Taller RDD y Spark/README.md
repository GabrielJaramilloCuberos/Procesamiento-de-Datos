# Taller RDD y Spark

Este directorio contiene el cuaderno [LabSpark02_Jaramillo.ipynb](LabSpark02_Jaramillo.ipynb). El taller practica el uso de Apache Spark desde PySpark: creación y manipulación de RDD, transformaciones, acciones, lectura de archivos y análisis básico con DataFrames.

## Contenido del cuaderno

| Bloque | Propósito |
| --- | --- |
| Configuración inicial | Comprueba las versiones de Python y PySpark, y define las variables necesarias para trabajar con el clúster. |
| Sesión de Spark | Crea una SparkSession y obtiene su SparkContext para ejecutar las prácticas. |
| RDD | Crea RDD vacíos, RDD desde listas y RDD a partir de rangos numéricos. |
| Particiones | Examina particiones con getNumPartitions y glom; practica repartition y coalesce. |
| Transformaciones y acciones | Usa map, filter, flatMap, reduce, reduceByKey, countByValue, collect, take y takeOrdered. |
| Archivos de texto | Lee archivos CSV como RDD y procesa sus líneas. |
| DataFrames | Carga el conjunto Wine Quality, convierte tipos, revisa nulos y aplica consultas. |

## Ambiente de ejecución

El cuaderno está configurado para ejecutarse con el kernel **Python 3.11 - Spark**. Antes de crear la sesión, se define el Python que usará el driver y el que usarán las tareas enviadas a los workers.

~~~python
import os
import sys

os.environ["SPARK_HOME"] = "/opt/cluster/spark"
os.environ["PYSPARK_PYTHON"] = "/usr/bin/python3.11"
os.environ["PYSPARK_DRIVER_PYTHON"] = sys.executable
~~~

Luego se construye la sesión. La dirección del master se deja como un valor de configuración del entorno, por lo que cada integrante debe usar la indicada para su clúster.

~~~python
from pyspark.sql import SparkSession

spark = (
    SparkSession.builder
    .appName("Spark_Jaramillo_00")
    .master("spark://MASTER:PUERTO")
    .config("spark.pyspark.python", "/usr/bin/python3.11")
    .config(
        "spark.scheduler.allocation.file",
        "file:///opt/cluster/spark/conf/fairscheduler.xml",
    )
    .config("spark.executor.memory", "1g")
    .config("spark.executor.cores", "2")
    .config("spark.cores.max", "2")
    .getOrCreate()
)

sc = spark.sparkContext
~~~

La coherencia de versión de Python es importante: el driver del notebook y los procesos Python de los workers deben usar una versión compatible con PySpark.

## Práctica con RDD

Un RDD es una colección distribuida que Spark puede dividir en particiones y procesar en paralelo. El cuaderno muestra varias maneras de crearlo:

~~~python
# RDD vacío
vacio = sc.parallelize([])

# RDD a partir de una lista
mixto = sc.parallelize([True, [11, 12, 13], (10, 20, 30)], 3)

# RDD con números del 1 al 3999 repartidos en cuatro particiones
rango = sc.parallelize(range(1, 4000), 4)
~~~

Para observar cómo se distribuyen los datos se usa glom:

~~~python
print(rango.getNumPartitions())
print(rango.glom().take(1))
~~~

Cada elemento devuelto por glom representa una partición como una lista. Por eso, al usar max o min sobre glom se comparan listas entre sí; no equivale necesariamente al máximo o mínimo global de los números.

El taller también diferencia dos operaciones de particionamiento:

- repartition puede aumentar o disminuir particiones y realiza una redistribución completa.
- coalesce se usa normalmente para reducir particiones con menos movimiento de datos.

Los RDD son inmutables. Operaciones como coalesce y repartition devuelven un RDD nuevo, por lo que el resultado debe asignarse si se desea conservarlo:

~~~python
rango_reducido = rango.coalesce(2)
~~~

## Transformaciones y acciones

Las transformaciones describen un procesamiento, pero Spark no lo ejecuta inmediatamente. Algunas usadas en el cuaderno son:

- map: aplica una función a cada elemento.
- filter: conserva los elementos que cumplen una condición.
- flatMap: transforma un elemento en varios elementos.
- reduceByKey: combina valores que tienen la misma clave.

Las acciones solicitan el resultado y desencadenan la ejecución. El cuaderno usa, entre otras, count, collect, take, reduce, countByValue y takeOrdered.

~~~python
palabras = sc.parallelize(["hola", "spark", "hadoop", "python"])
con_h = palabras.filter(lambda palabra: palabra.startswith("h"))
mayusculas = palabras.map(lambda palabra: palabra.upper())
~~~

collect debe reservarse para resultados pequeños porque transfiere todos los datos al proceso del notebook. Para inspeccionar conjuntos grandes conviene usar take:

~~~python
vista = rango.take(10)
~~~

## Lectura de archivos

El cuaderno trabaja con estos archivos:

| Archivo | Uso en el taller |
| --- | --- |
| test.csv | Ejercicio de lectura de texto y conteo por línea. |
| train.csv | Separación de columnas y selección de campos de un CSV como RDD. |
| winequality-red.csv | Carga y análisis tabular mediante DataFrame. |

Las rutas de lectura configuradas son:

~~~text
/opt/cluster/documentos/test.csv
/opt/cluster/documentos/train.csv
/opt/cluster/documentos/winequality-red.csv
~~~

Antes de ejecutar esas celdas se debe comprobar que los archivos existan:

~~~bash
ls -lh /opt/cluster/documentos/
~~~

Cuando se usa una ruta que inicia con file:///, Spark busca ese archivo en el sistema de archivos de cada worker que necesite procesarlo. Por ello, la ubicación debe estar disponible para los workers, ya sea porque es un directorio compartido o porque el archivo fue distribuido adecuadamente. El segundo argumento de textFile indica el número de particiones deseado; no copia el archivo a otros equipos.

Para un sistema HDFS configurado, la ruta debe ser una ruta HDFS válida y distinta de la interfaz web. Es útil verificar su configuración y contenido con:

~~~bash
/opt/cluster/hadoop/bin/hadoop fs -ls /
/opt/cluster/hadoop/bin/hadoop fs -ls /ruta/del/archivo
~~~

## Procesamiento de CSV como RDD

Los CSV se leen inicialmente como líneas de texto. Después, las líneas se dividen por su separador y se seleccionan las columnas necesarias.

~~~python
train = sc.textFile("file:///opt/cluster/documentos/train.csv", 4)

campos = train.map(lambda linea: linea.split(","))
ejemplo = campos.map(lambda fila: (fila[5], fila[1]))
~~~

Antes de indexar posiciones de una fila conviene revisar algunas líneas con take para confirmar el separador, la cabecera y el número de columnas.

## DataFrames y Wine Quality

En la parte final se carga el archivo winequality-red.csv con cabecera y separador punto y coma:

~~~python
df_wine = (
    spark.read
    .option("header", True)
    .option("sep", ";")
    .option("inferSchema", True)
    .csv("file:///opt/cluster/documentos/winequality-red.csv")
)
~~~

Con el DataFrame se realizan las siguientes actividades:

1. Se inspeccionan las columnas y el esquema.
2. Se convierten las medidas del vino a tipos numéricos apropiados.
3. Se consultan estadísticas descriptivas con describe.
4. Se buscan valores nulos y NaN después de la conversión numérica.
5. Se seleccionan, ordenan y filtran columnas como pH, densidad y calidad.
6. Se convierte una muestra limitada a pandas para visualización local.

~~~python
df_wine.describe().show()

faltantes = df_wine.select([
    F.count(F.when(F.isnan(columna) | F.col(columna).isNull(), columna)).alias(columna)
    for columna in df_wine.columns
])
faltantes.show()
~~~

La verificación con isnan debe aplicarse a columnas numéricas. Por esa razón el cuaderno realiza primero el casteo de las variables del conjunto Wine Quality.

## Dependencias

El cuaderno utiliza principalmente:

- Python 3.11
- Apache Spark y PySpark
- NumPy
- pandas
- Matplotlib
- Seaborn
- findspark
- PyArrow, únicamente para conversiones a pandas cuando sea necesario

Si falta alguna dependencia, debe instalarse en el mismo kernel con el que se ejecuta el cuaderno. Por ejemplo:

~~~python
%pip install seaborn pyarrow findspark
~~~

Después de instalar paquetes puede ser necesario reiniciar el kernel antes de importarlos.

## Cierre de recursos

Al finalizar la práctica, el cuaderno detiene explícitamente la sesión:

~~~python
spark.stop()
~~~

Esto libera los recursos solicitados al clúster. Si se desea continuar ejecutando más celdas de Spark después de ese punto, debe crearse una sesión nueva.

## Archivos de datos

Los archivos CSV no se incluyen en este repositorio, porque se administran en el almacenamiento del entorno de práctica. Cada integrante debe verificar su disponibilidad antes de ejecutar las secciones que los requieren.
