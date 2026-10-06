# Taller RDD y Spark

Este taller introduce Apache Spark mediante PySpark. El cuaderno desarrolla la creación de RDD, particiones, transformaciones, acciones, funciones definidas por el usuario, lectura de archivos distribuidos y operaciones básicas con DataFrames.

El material principal es [LabSpark02_Jaramillo.ipynb](LabSpark02_Jaramillo.ipynb).

## Nota:
El taller fue realizado en el clúster del grupo estudiantil.

## Contenido del taller

| Sección | Tema | Qué se practica |
|---|---|---|
| 1 | Bibliotecas | Importación de PySpark, funciones SQL, tipos de datos y bibliotecas de análisis. |
| 2 | Sesión Spark | Creación de \`SparkSession\`, \`SparkContext\` y \`SQLContext\`. |
| 3 | RDD | RDD vacíos, \`parallelize\`, particiones, \`glom\`, nombres y rangos. |
| 4 | Operaciones RDD | Transformaciones, acciones, evaluación perezosa y DAG. |
| 5 | Funciones de usuario | Uso de funciones propias y expresiones \`lambda\` con \`filter\` y \`map\`. |
| 6 | HDFS | Exploración del sistema de archivos distribuido y lectura de archivos de texto. |
| 7 | Operaciones adicionales | Replicación de listas, conteos y \`countByValue\`. |
| 8 | Titanic desde archivo | Separación de texto, creación de pares y selección de campos. |
| 9 | DataFrames | Creación, lectura CSV, tipos, descriptivos, selección, ordenamiento, filtros y subconjuntos. |
| 10 | Cierre | Liberación de recursos con \`spark.stop()\`. |

## Conceptos principales

### Spark, driver y workers

Spark distribuye tareas entre los workers disponibles en el cluster. El notebook se ejecuta desde el driver, que planifica el trabajo y recibe los resultados de las acciones. Los workers ejecutan las tareas sobre las particiones.

### RDD

Un RDD es un conjunto de datos distribuido, inmutable y tolerante a fallos. Se divide en particiones para procesarse en paralelo.

\`\`\`python
# Crea un RDD con valores del 1 al 3999 en cuatro particiones.
a = spark_context.parallelize(range(1, 4000), 4)

# Consulta el número de particiones.
a.getNumPartitions()
\`\`\`

\`glom()\` permite observar cada partición como una lista. Es útil para aprender cómo Spark reparte los elementos, pero no conviene usarlo con datasets grandes porque puede traer demasiados datos al driver.

### Transformaciones y acciones

Las transformaciones construyen un nuevo RDD sin ejecutar inmediatamente el cálculo. Por ejemplo:

\`\`\`python
mayusculas = palabras_rdd.map(lambda palabra: palabra.upper())
inician_h = palabras_rdd.filter(lambda palabra: palabra.lower().startswith("h"))
\`\`\`

Las acciones solicitan resultados y desencadenan la ejecución del plan de Spark:

\`\`\`python
mayusculas.take(10)   # Obtiene como máximo diez elementos.
mayusculas.count()    # Cuenta elementos.
mayusculas.collect()  # Lleva todos los elementos al driver.
\`\`\`

Se recomienda usar \`take(n)\` durante las pruebas. \`collect()\` solo es apropiado cuando se sabe que el resultado es pequeño, pues intenta copiar todo al notebook.

### Particiones, \`repartition\` y \`coalesce\`

El taller muestra cómo cambiar el número de particiones:

\`\`\`python
nuevo = rdd.repartition(7)
reducido = rdd.coalesce(2)
\`\`\`

\`repartition(n)\` puede aumentar o reducir particiones y normalmente implica una redistribución de datos. \`coalesce(n)\` se utiliza usualmente para reducir particiones con menos movimiento de datos. Ambos devuelven un nuevo RDD, ya que los RDD son inmutables.

### Funciones propias y \`lambda\`

El cuaderno compara una función definida por el usuario con una función anónima:

\`\`\`python
def mayuscula(texto):
    return texto.upper()

rdd.map(mayuscula)
rdd.map(lambda texto: texto.upper())
\`\`\`

Ambas alternativas transforman cada elemento. La función con nombre resulta más clara cuando la lógica crece; \`lambda\` es cómoda para expresiones breves.

## HDFS y archivos locales

Spark puede leer archivos desde HDFS, almacenamiento local compartido, nube u otros sistemas compatibles. El prefijo de la ruta cambia según el sistema de archivos:

\`\`\`python
# HDFS: usa la configuración activa del cluster.
rdd = spark_context.textFile("hdfs:///ruta/en/hdfs/archivo.csv")

# Archivo local: la ruta debe existir en cada worker que procese particiones.
rdd = spark_context.textFile("file:///ruta/compartida/archivo.csv")
\`\`\`

El material original trae rutas HDFS de ejemplo. Deben reemplazarse por la ruta real del NameNode y del archivo disponible en el entorno actual. El puerto de una interfaz web de Spark no sirve como puerto HDFS.

Para revisar HDFS desde una terminal del cluster se pueden usar comandos como:

\`\`\`bash
hdfs getconf -confKey fs.defaultFS
hdfs dfs -ls /
hdfs dfs -ls /ruta/en/hdfs
\`\`\`

Si se usa \`file:///\`, el archivo debe existir en la misma ruta de todos los workers o estar en un directorio realmente compartido. Solicitar cuatro particiones en \`textFile(..., 4)\` no copia ni distribuye el archivo automáticamente.

## DataFrames

La última parte del taller utiliza DataFrames de Spark. Primero crea un DataFrame manualmente y luego ilustra la lectura de un CSV, el cambio de tipos, los descriptivos y los filtros.

\`\`\`python
df = spark.createDataFrame(
    [("Pasta", 100), ("Pizza", 200)],
    ["Comida", "Precio"]
)

df.show()
\`\`\`

Al leer CSV, Spark puede iniciar las columnas como texto. Por eso el taller crea una nueva versión con tipos numéricos para calcular estadísticas y filtros. Conservar una versión original ayuda a revisar conversiones y evita perder trazabilidad.

Ejemplos de operaciones incluidas:

\`\`\`python
# Ver esquema y estadísticas.
df.printSchema()
df.describe().show()

# Seleccionar y ordenar columnas.
df.select("columna_1", "columna_2").orderBy("columna_2").show()

# Filtrar mediante condiciones.
df.filter((F.col("pH") > 3) & (F.col("sulphates") < 0.6)).show()
\`\`\`

## Preparación del entorno

El taller requiere un kernel compatible con la versión de Spark instalada en el cluster. Para el entorno del curso se recomienda Python 3.11 y PySpark 4.2.0. Antes de crear la sesión, se debe asegurar que tanto el driver como los workers utilicen Python 3.11:

\`\`\`python
import os
import sys

os.environ["SPARK_HOME"] = "/opt/cluster/spark"
os.environ["PYSPARK_PYTHON"] = "/usr/bin/python3.11"
os.environ["PYSPARK_DRIVER_PYTHON"] = sys.executable
\`\`\`

Después se puede crear una única sesión con la configuración acordada para el cluster. No se debe crear más de un \`SparkContext\` activo en el mismo kernel.

\`\`\`python
from pyspark.sql import SparkSession

spark = (
    SparkSession.builder
    .appName("Taller_RDD_y_Spark")
    .master("spark://DIRECCION_DEL_MASTER:PUERTO")
    .config("spark.pyspark.python", "/usr/bin/python3.11")
    .getOrCreate()
)

spark_context = spark.sparkContext
\`\`\`

La dirección del master debe obtenerse de la configuración del curso o del equipo. No se incluye en este README.

## Ajustes necesarios en el cuaderno

El cuaderno reúne ejemplos de distintas versiones y autores. Antes de ejecutarlo de principio a fin, conviene unificar los nombres de las variables para referirse siempre a la misma sesión y contexto:

\`\`\`python
spark
spark_context
\`\`\`

Por ejemplo, sustituir referencias como \`sparkContextoFranco\`, \`sparkContextoCorredor\`, \`sparkContextoCorrer\` o variantes con errores de escritura por \`spark_context\`. Del mismo modo, usar siempre \`spark\` para crear DataFrames.

Los datos de Titanic y Wine Quality no se incluyen en esta carpeta. Para ejecutar esas secciones deben existir en HDFS o en un directorio compartido y se deben ajustar las rutas de lectura. La sección de SQL se menciona en los objetivos del material, aunque el cuaderno actual no incluye una práctica explícita con \`spark.sql()\`.

## Cierre de la sesión

Al terminar el taller, liberar los recursos de la aplicación:

\`\`\`python
spark.catalog.clearCache()
spark.stop()
\`\`\`

Después de \`spark.stop()\`, las variables RDD y DataFrame dejan de poder ejecutar acciones. Para continuar, se debe crear o recuperar una sesión nueva.

