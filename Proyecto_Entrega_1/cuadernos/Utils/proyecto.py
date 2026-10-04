#<------------------------------------------------------->#
# Nombres:                                                #
# - Gabriel Jaramillo Cuberos (Id: 20529022)              #
#                                                         #
# - David Vargas (Id: )                                   #
# - Juan Felipe Gómez López (Id: 20553416)                #
# - Sara Pulgarín (Id: 20491129)                          #
# - Juan Camilo (Id: )                                    #
#                                                         #
# Materia: Procesamiento de Datos (1255)                  #
# Nombre del fichero: 01_Arrestos.ipynb                   #
# Descripción: Este archivo contiene las funciones        #
#              compartidas entre cuadernos.               #
#<------------------------------------------------------->#

from pathlib import Path
from datetime import datetime, timezone
import json
import os
import re
import sys


# Constantes de configuración
ANIO = 2018 # Año objetivo del análisis
DATA_URI = "file:////opt/cluster/Proyecto/Entrega_1/datasets/"  # URI de archivos locales
DATA_DIR = Path("/opt/cluster/Proyecto/Entrega_1/datasets")  # Carpeta de archivos locales como Path
RESULTADOS = Path("/opt/cluster/Proyecto/Entrega_1/resultados")  # Carpeta donde se guardan salidas
 
# Diccionarios de mapeo para normalizar códigos
BOROUGHS = ["Bronx", "Brooklyn", "Manhattan", "Queens", "Staten Island"]
BORO_ARRESTOS = {"B": "Bronx", "K": "Brooklyn", "M": "Manhattan", "Q": "Queens", "S": "Staten Island"}  # Códigos usados en el dataset de arrestos
BORO_POBREZA = {"1": "Bronx", "2": "Brooklyn", "3": "Manhattan", "4": "Queens", "5": "Staten Island"}  # Códigos usados en el dataset de pobreza
EDUCACION = {1: "Menos de secundaria", 2: "Secundaria completa", 3: "Estudios superiores sin bachelor", 4: "Bachelor o superior"}  # Códigos numéricos de nivel educativo a texto
 
 
def iniciar_spark(nombre):
    
    # Verifica que el kernel activo sea Python 3.11
    if sys.version_info[:2] != (3, 11):
        raise RuntimeError("Selecciona el kernel de jupyter-spark-env (Python 3.11) y reinicia el kernel.")
 
    # Variables de entorno que le dicen a PySpark dónde está Spark y qué intérprete de Python usar
    os.environ["SPARK_HOME"] = "/opt/cluster/spark"
    os.environ["PYSPARK_PYTHON"] = "/usr/bin/python3.11"
    os.environ["PYSPARK_DRIVER_PYTHON"] = sys.executable
 
    # Si existe un JDK 17 local, se fija JAVA_HOME
    if Path("/usr/lib/jvm/java-17-openjdk").exists():
        os.environ["JAVA_HOME"] = "/usr/lib/jvm/java-17-openjdk"
 
    from pyspark.sql import SparkSession
    import pyspark
    global F
    from pyspark.sql import functions as F
 
    # Sobreescribe la URL del master del clúster con un archivo de configuración local
    config_path = Path(__file__).with_name("config_cluster.local.json")
    local = json.loads(config_path.read_text(encoding="utf-8")) if config_path.exists() else {}
    master = os.environ.get("SPARK_MASTER_URL") or local.get("master")
 
    # Configuración del fair scheduler
    # si no, se cae al modo FIFO por defecto de Spark.
    allocation = Path("/opt/cluster/spark/conf/fairscheduler.xml")
    builder = (SparkSession.builder.appName(nombre).master(master)
               .config("spark.executor.memory", "1g") # Limita memoria para no acaparar recursos
               .config("spark.executor.cores", "2") # Establece núcleos por ejecutor
               .config("spark.cores.max", "2") # Establece número de nucleos
               .config("spark.pyspark.python", "/usr/bin/python3.11")
               .config("spark.sql.shuffle.partitions", "8")  # Evita overhead
               .config("spark.sql.session.timeZone", "America/New_York")  # Todas las fechas se interpretan en esta zona horaria
               .config("spark.scheduler.mode", "FAIR" if allocation.exists() else "FIFO")
               .config("spark.scheduler.allocation.file", allocation.as_uri()))
    spark = builder.getOrCreate()  # Reutiliza una sesión existente si ya hay una activa en este proceso. En caso contrario, crea una nueva
 
    # Comprueba si existe otra sesión
    conf = spark.sparkContext.getConf()
    expected = {"spark.master": master, "spark.cores.max": "2", "spark.executor.cores": "2",
                "spark.pyspark.python": "/usr/bin/python3.11"}
    if any(conf.get(k, "") != v for k, v in expected.items()):
        raise RuntimeError("Ya existía otra sesión con diferente configuración. Reinicia el kernel y ejecuta desde el principio.")
 
    # Verifica la versión de PySpark
    if pyspark.__version__.split('.')[:2] != spark.version.split('.')[:2]:
        raise RuntimeError("PySpark y Spark tienen versiones distintas. Usa el kernel del taller con PySpark 4.2.0.")
 
    spark.sparkContext.setLogLevel("WARN") # Reduce logs
    RESULTADOS.mkdir(parents=True, exist_ok=True) # Asegura que exista la carpeta de salida para evitar errores
 
    # Muestra información del entorno y kernel
    print("Python:", sys.version.split()[0], "| PySpark:", pyspark.__version__, "| Spark:", spark.version)
    print("Ejecutable:", sys.executable)
    print("Aplicación:", spark.sparkContext.applicationId, "| Máximo de núcleos: 2")
    print("Los recursos físicos del cluster deben documentarse desde la interfaz del master.")
    return spark

# Convierte un nombre de columna arbitrario
def normalizar(nombre):
    return re.sub(r"[^a-z0-9]+", "_", nombre.strip().lower()).strip("_")
 
# Lee los datasets
def leer_csv(spark, archivo, requeridas):
    # Falla rápido y con mensaje si no se encuentra el archivo
    if not (DATA_DIR / archivo).is_file():
        raise FileNotFoundError(f"Sube {archivo} a {DATA_DIR}. La misma ruta debe ser accesible por los workers.")
 
    df = (spark.read.option("header", True)
          .option("inferSchema", False)
          .option("encoding", "UTF-8")
          .option("mode", "FAILFAST") # Si una fila está mal formada, falla en vez de descartarla silenciosamente o corromper datos
          .option("quote", '"').option("escape", '"') # Manejo estándar de comillas o escapes en CSV
          .csv(DATA_URI + archivo))
 
    nombres = [normalizar(c) for c in df.columns]
    if len(set(nombres)) != len(nombres):
        # Si dos columnas distintas normalizan al mismo nombre se detecta la coincidencia
        raise ValueError("Hay nombres de columnas duplicados al normalizar.")
    df = df.toDF(*nombres)  # Renombra las columnas del DataFrame con las versiones normalizadas
 
    faltan = set(requeridas) - set(nombres)
    if faltan: # Verifica que todas las columnas que el notebook necesita existan en el archivo
        raise ValueError(f"Columnas ausentes en {archivo}: {sorted(faltan)}") 
    return df
 
# Limpia textos en los datasets
def texto(c):
    value = F.trim(F.col(c).cast("string"))
    return F.when(value.isNull() | F.lower(value).isin("", "null", "(null)", "nan"), None).otherwise(value)

# Modifica el formato de los números para evitar errores con miles y decimales
def numero(c, espanol=False):
    value = texto(c)
    if espanol:
        value = F.regexp_replace(F.regexp_replace(value, r"\.", ""), ",", ".")
    return value.try_cast("double")  # try_cast devuelve null en vez de lanzar error si el valor no es numérico.
 
# Parsea la columna con varios formatos de fechas y usa el primero que funcione
def fecha(c):
    return F.coalesce(*[
        F.try_to_timestamp(texto(c), F.lit(formato)) for formato in
        ["MM/dd/yyyy", "yyyy-MM-dd", "yyyy-MM-dd'T'HH:mm:ss.SSS", "yyyy-MM-dd'T'HH:mm:ss"]
    ]).cast("date")
 
# Crea un mapa para traducir valores y aplanar pares de datos
def mapa(c, mapping):
    return F.create_map(*[F.lit(x) for pair in mapping.items() for x in pair])[F.col(c)]
 
# Calcula la calidad del dataset usado
def calidad(df):
    # Calcula, para cada columna, cuántos valores están vacíos y qué porcentaje representan sobre el total de filas
    n = df.count()
    row = df.agg(*[F.sum(F.when(texto(c).isNull(), 1).otherwise(0)).alias(c) for c in df.columns]).first()
    return [{"columna": c, "faltantes": int(row[c] or 0), "total": n,
             "porcentaje": round(100 * (row[c] or 0) / n, 3) if n else None} for c in df.columns]
 
# Guarda datos en formato JSON
def guardar_json(nombre, datos):
    RESULTADOS.mkdir(parents=True, exist_ok=True)
    p = RESULTADOS / nombre
    tmp = p.with_suffix(p.suffix + ".tmp")
    tmp.write_text(json.dumps(datos, ensure_ascii=False, indent=2, allow_nan=False, default=str), encoding="utf-8")
    tmp.replace(p)
    return p
 
# Muestra una lista de diccionarios como una tabla HTML dentro del notebook
def tabla(registros, limite=30):
    from IPython.display import display, HTML
    from html import escape
    rows = list(registros)
    if not rows:
        print("Sin registros para mostrar.")
        return
    cols = list(rows[0])  # Las columnas se toman de las claves del primer registro.
    
    # Se escapa todo el contenido para evitar datos con caracteres especiales
    content = '<table><thead><tr>' + ''.join('<th>'+escape(str(c))+'</th>' for c in cols) + '</tr></thead><tbody>'
    for row in rows[:limite]:  # Se muestra un resultado limitado para no saturar salidas
        content += '<tr>' + ''.join('<td>'+escape(str(row.get(c, '')) if row.get(c) is not None else 'No disponible')+'</td>' for c in cols) + '</tr>'
    display(HTML(content+'</tbody></table>'))
    if len(rows)>limite:
        print(f"Se muestran {limite} de {len(rows)} filas; la salida guardada contiene todas.")
 
# Trae del clúster al driver un DataFrame de Spark como lista de diccionarios de Python
def filas(df, limite=1000):
    result = df.limit(limite+1).collect()
    if len(result)>limite:
        raise ValueError("Solo deben recopilarse tablas agregadas pequeñas en el notebook.")
    return [r.asDict(recursive=True) for r in result]
 
# Genera y guarda un gráfico de barras horizontales a partir de una lista de diccionarios
def barras(registros, categoria, valor, titulo, archivo):
    import matplotlib.pyplot as plt
    datos = [r for r in registros if r.get(valor) is not None]  # Descarta registros sin valor numérico.
    if not datos:
        print(titulo + ": no hay datos disponibles para este período.")
        return
    # Se aumenta el alto de la figura de acuerdo a la cantidad de barras
    fig, ax = plt.subplots(figsize=(10, max(4, len(datos)*0.36)))
    ax.barh([str(r.get(categoria) or "Sin dato") for r in datos], [r[valor] for r in datos], color="#23658a")
    ax.invert_yaxis()
    ax.set_title(titulo, loc="left", pad=14)
    ax.set_xlabel(valor.replace("_", " "))
    ax.grid(axis="x", alpha=0.2)
    ax.set_axisbelow(True)
    fig.tight_layout()
    RESULTADOS.mkdir(parents=True, exist_ok=True)
    fig.savefig(RESULTADOS / archivo, dpi=160, bbox_inches="tight") # Guarda la imagen en la carpeta de resultados
    plt.show()
    plt.close(fig) # Libera la memoria de la figura
 
# Muestra y guarda el diccionario de datos de un DataFrame
def diccionario(df, fuente):
    dic = json.loads(Path(__file__).with_name("diccionarios.json").read_text(encoding="utf-8"))[fuente]
    entries = [{"atributo": c, "tipo_leido": "string (CSV original)",
                "significado": dic.get(c, {}).get("significado", "Consultar diccionario oficial"),
                "codigos_notas": dic.get(c, {}).get("codigos", "")} for c in df.columns]
    tabla(entries, limite=100)
    guardar_json("diccionario_"+fuente+".json", entries)
 
# Limpieza de duplicados basada en una o más columnas, solo quita duplicados exactos
def depurar_identificador(df, claves):
    original = df.count()
    unicos = df.dropDuplicates().cache()
    n_unicos = unicos.count()
 
    validas = unicos
    for clave in claves:
        validas = validas.filter(texto(clave).isNotNull()) # Descarta filas sin valor en alguna de las columnas
    n_validas = validas.count()
 
    # Encuentra combinaciones de clave que aparecen más de una vez
    conflictos = validas.groupBy(*claves).count().filter(F.col("count")>1).select(*claves)
    
    # Excluye conflictos
    limpias = validas.join(conflictos, claves, "left_anti").cache()
    n_limpias = limpias.count()
 
    # Genera un reporte de auditoría con la cantidad de filas perdidas en el proceso
    auditoria = {"filas_originales": original, "duplicados_exactos": original-n_unicos,
                 "filas_sin_clave": n_unicos-n_validas,
                 "filas_con_clave_conflictiva_excluidas": n_validas-n_limpias,
                 "filas_conservadas": n_limpias}
    unicos.unpersist() # Libera cache
    return limpias, auditoria
 
# Cuenta cuántas filas hay por año en la columna de fecha indicada
def cobertura(df, c="fecha"):
    return filas(df.groupBy(F.year(c).alias("anio")).count().orderBy("anio"))
 
# Exporta un DataFrame ya agregado a un JSON en resultados, junto con metadatos que permiten verificar más adelante si sigue siendo válido
def exportar_resumen(df, nombre, fuente, estado, notas):
    rows = filas(df)
    if rows and any(r.get("anio") != ANIO for r in rows):
        raise ValueError("El resumen contiene años diferentes al objetivo.") # Verificación de consistencia
    path = DATA_DIR / fuente
    return guardar_json(nombre, {"version": 1, "anio_objetivo": ANIO, "estado": estado,
        "fuente": fuente,
        "tamano_fuente": path.stat().st_size, # Obtiene tamaño del CSV
        "modificacion_fuente_ns": path.stat().st_mtime_ns,
        "fecha_ejecucion_utc": datetime.now(timezone.utc).isoformat(),
        "notas": notas,
        "schema": df.schema.jsonValue(), # Se guarda el schema de Spark en JSON para poder reconstruir el DataFrame
        "datos": rows})
 
# Carga de vuelta un resumen previamente exportado
def cargar_resumen(spark, nombre):
    p = RESULTADOS / nombre
    if not p.exists():
        raise FileNotFoundError(f"Falta {p.name}. Ejecuta primero el cuaderno que lo produce.")
    obj = json.loads(p.read_text(encoding="utf-8"))
 
    if obj["anio_objetivo"] != ANIO:
        # Genera error si el resumen guardado es de otro año
        raise ValueError(f"{nombre} pertenece a otro período. Reejecuta el cuaderno de origen.")
 
    # Verifica que el archivo fuente original no haya cambiado desde que se generó el resumen
    fuente = DATA_DIR / obj["fuente"]
    if not fuente.exists() or fuente.stat().st_size != obj["tamano_fuente"] or fuente.stat().st_mtime_ns != obj["modificacion_fuente_ns"]:
        raise ValueError(f"Cambió el archivo {obj['fuente']}. Reejecuta su cuaderno para actualizar el resumen.")
 
    from pyspark.sql.types import StructType
    df = spark.createDataFrame(obj["datos"], schema=StructType.fromJson(obj["schema"]))
    return df, obj