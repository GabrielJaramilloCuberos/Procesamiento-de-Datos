# Proyecto de procesamiento de datos · Entrega 1

Este proyecto documenta la carga, exploración, revisión de calidad, preparación e integración de datos de la ciudad de Nueva York (NYC), con 2018 como año de referencia. El procesamiento principal se realiza con PySpark en un cluster y se presenta mediante cuadernos de Jupyter con explicaciones, tablas y gráficas.

Se trabajan tres conjuntos de datos: arrestos, vehículos involucrados en colisiones y pobreza. El perfil educativo se obtiene del propio dataset de pobreza. La integración incorpora además población por distrito obtenida mediante extracción web.

## Estructura de la carpeta

```text
Proyecto_Entrega_1/
├── README.md
├── Bonos/
│   ├── 01_Bono_NYHealth.ipynb
│   └── 02_Bono_ClimaAPI.ipynb
├── cuadernos/
│   ├── 01_Arrestos.ipynb
│   ├── 02_Vehiculos_Accidentes.ipynb
│   ├── 03_Pobreza_Educacion.ipynb
│   ├── 04_Integracion_Preguntas.ipynb
│   └── 05_Bono_Scraper.ipynb
└── resultados/
    ├── Reportes de calidad y limpieza en JSON
    ├── Diccionarios de atributos en JSON
    ├── Resúmenes e indicadores en JSON
    └── Gráficas en PNG
```

Los datasets originales no se incluyen en el repositorio debido al tamaño de los archivos. Deben descargarse y ubicarse en el entorno de ejecución antes de procesarlos. Los resultados son archivos derivados de menor tamaño; no sustituyen los datos originales.

## Cuadernos y responsabilidades

### 01 · Arrestos

[Abrir cuaderno](cuadernos/01_Arrestos.ipynb)

- Lee `Arrestos_NY.csv` y presenta el esquema y el significado de sus atributos.
- Cuenta valores faltantes y calcula sus porcentajes por columna.
- Revisa los años disponibles y el rango de fechas antes de seleccionar 2018.
- Aplica la depuración por `arrest_key` y registra sus resultados en una auditoría.
- Traduce los códigos territoriales a nombres de distritos.
- Presenta la distribución de arrestos por grupo de edad y por distrito.
- Exporta `arrestos_resumen.json`, agrupado por año y distrito.

La unidad analizada es el evento de arresto; el conteo no equivale necesariamente a personas diferentes. El archivo de entrada debe contener el período de 2018: el nombre del CSV, por sí solo, no garantiza su cobertura.

### 02 · Vehículos involucrados en colisiones

[Abrir cuaderno](cuadernos/02_Vehiculos_Accidentes.ipynb)

- Lee `Accidentes_Viales_NY.csv`, correspondiente a la tabla de vehículos.
- Describe sus atributos, faltantes y cobertura temporal, y selecciona 2018.
- Aplica la depuración por `unique_id` y registra los casos sin identificador de colisión.
- Prepara etiquetas de tipo de vehículo y primer factor contribuyente reportado; utiliza `SIN DATO` cuando falta información en estas columnas derivadas.
- Distingue entre filas de vehículos y colisiones identificables mediante `collision_id`.
- Describe los vehículos registrados por colisión, los tipos de vehículo más frecuentes y la disponibilidad del primer factor reportado.
- Genera `vehiculos_resumen.json` como resumen global del período.

Una colisión puede involucrar varios vehículos y aparecer en varias filas. Este archivo no contiene distrito ni cantidades de lesionados o fallecidos, por lo que su resumen no se distribuye territorialmente en la integración.

### 03 · Pobreza y educación

[Abrir cuaderno](cuadernos/03_Pobreza_Educacion.ipynb)

- Lee `Pobreza_NY.csv`, describe sus atributos y evalúa los faltantes.
- Aplica la depuración mediante la clave compuesta `serialno` y `sporder`.
- Crea columnas numéricas conservando las originales y registra conversiones fallidas. La conversión considera el formato numérico del archivo, con coma decimal y punto de miles.
- Audita pesos inválidos, distritos desconocidos y categorías de pobreza no válidas.
- Distingue el número de registros de la muestra de la población representada por la suma de `PWGTP`.
- Presenta edades mínima y máxima, distribución ponderada por edad, nivel educativo y porcentaje ponderado de pobreza.
- Exporta `pobreza_resumen.json` con indicadores por año y distrito.

`PWGTP` es el peso de persona: indica cuánta población representa cada registro. Las estimaciones emplean pesos positivos. La pobreza se calcula entre registros con clasificación válida y considera todas las edades; la descripción educativa utiliza personas de 25 años o más con categoría educativa válida. Los denominadores de ambos indicadores son distintos. Se presentan estimaciones puntuales, sin intervalos de confianza.

### 04 · Integración territorial

[Abrir cuaderno](cuadernos/04_Integracion_Preguntas.ipynb)

- Carga los resúmenes de arrestos, vehículos y pobreza, junto con sus metadatos.
- Lee `resultados/poblacion_nyc_2018.csv`, generado por el cuaderno 5.
- Valida año, nombres de distritos, poblaciones y ausencia de claves territoriales duplicadas.
- Construye una base con los cinco distritos: Bronx, Brooklyn, Manhattan, Queens y Staten Island.
- Une población, pobreza y arrestos por `anio` y `borough`, conservando los cinco distritos con uniones izquierdas.
- Calcula y muestra la tasa de arrestos por cada 100.000 habitantes.
- Guarda la disponibilidad de indicadores y la tabla territorial integrada.

```text
arrestos_por_100mil = arrestos / poblacion × 100.000
```

La integración relaciona indicadores agregados de un territorio, no personas individuales entre datasets. Los valores nulos representan información no disponible y no se convierten en ceros. La tabla de disponibilidad indica presencia de valores, no garantiza cobertura completa. El resumen de vehículos se carga para documentar su estado, pero no participa en la unión por distrito.

## Bonos y extracción complementaria

La carpeta `Bonos` contiene dos actividades: la extracción de población de NY Health y la consulta meteorológica mediante la API de OpenWeather. Además, en `cuadernos` se mantiene el scraper de NYCdata, cuya salida es la que utiliza la integración territorial.

### Población: diferencia entre NY Health y NYCdata

| Aspecto | Bono de NY Health | Bono de población de NYCdata |
|---|---|---|
| Ubicación | `Bonos/01_Bono_NYHealth.ipynb` | `cuadernos/05_Bono_Scraper.ipynb` |
| Procedencia | Enlace proporcionado por el profesor, del portal de datos de salud del estado de Nueva York. | Página de población de NYCdata, Baruch College. |
| Método | Descarga directa del CSV publicado por el portal mediante una petición HTTP. | Localiza en `TableMaker.js` el CSV que alimenta la tabla de la página y lo descarga. |
| Preparación | Selección de los cinco counties de NYC, preparación de totales y correspondencia entre county y borough. | Selección de NYC y 2018, normalización de nombres y conversión de miles de habitantes a habitantes. |
| Presentación y salida | Descarga en `datos/poblacion_ny_por_county.csv`, relativa al directorio de ejecución; presenta una tabla y una gráfica. | Guarda `resultados/poblacion_nyc_2018.csv`, con las columnas `anio`, `borough` y `poblacion`. |
| Relación con el cuaderno 4 | Actividad independiente; no alimenta la integración actual. | **Es el bono utilizado por el cuaderno de integración.** |

Fuentes utilizadas por los bonos:

- [CSV de NY Health utilizado en el bono del profesor](https://health.data.ny.gov/api/views/e9uj-s3sf/rows.csv?accessType=DOWNLOAD).
- [Tabla de población de NYCdata utilizada por el cuaderno 5](https://www.baruch.cuny.edu/nycdata/population-geography/population.htm).

El cuaderno 5 ya multiplica por 1.000 las cifras expresadas en miles. El cuaderno 4 recibe habitantes y no repite esa conversión. Como la fuente publica cifras redondeadas a miles, las tasas resultantes son aproximadas.

### Segundo bono de la carpeta Bonos · Consulta meteorológica con API

[Abrir cuaderno](Bonos/02_Bono_ClimaAPI.ipynb)

`02_Bono_ClimaAPI.ipynb` obtiene datos de OpenWeather para una ubicación de la ciudad de Nueva York. Realiza consultas a los endpoints `/weather` y `/forecast`: condición meteorológica actual y pronóstico de cinco días en intervalos de tres horas. Es una consulta a una API con autenticación, diferente de las extracciones de población anteriores.

El procedimiento implementado comprende:

- Consultar la API con una clave válida, controlar errores de autenticación y reintentar cuando se alcanza el límite de peticiones.
- Guardar las respuestas originales en JSON, junto con la fecha de consulta y las unidades utilizadas.
- Convertir las respuestas en tablas de condición actual y pronóstico con pandas.
- Preparar fechas y variables como temperatura, sensación térmica, humedad, presión, nubosidad, viento y precipitación.
- Eliminar duplicados del pronóstico por ciudad y fecha-hora, y ordenar los registros.
- Construir un resumen diario con medias, mínimos, máximos, precipitación acumulada y condición más frecuente. Se consideran completos los días con ocho registros de tres horas.
- Revisar faltantes, duplicados y tipos de datos; visualizar temperatura, sensación térmica y probabilidad de lluvia. La gráfica de temperaturas diarias utiliza los días completos.

Sus archivos se generan en `datos/`, relativa al directorio desde el que se ejecuta el cuaderno:

| Archivo | Contenido |
|---|---|
| `respuestas_openweather.json` | Respuestas originales de condición actual y pronóstico, con metadatos de consulta. |
| `clima_ny_actual.csv` | Condición meteorológica actual de la ubicación consultada. |
| `clima_ny_pronostico_3h.csv` | Pronóstico preparado en intervalos de tres horas. |
| `clima_ny_diario.csv` | Indicadores agregados por día y marca de día completo. |

Las gráficas se muestran dentro del cuaderno. Estas salidas corresponden al momento de consulta y al horizonte del pronóstico, **no al año 2018**. El bono no alimenta el cuaderno 4 ni se cruza con los registros históricos de arrestos, vehículos o pobreza. Los archivos generados no se guardan automáticamente en la carpeta común `resultados`.

## Comunicación y orden de ejecución

Los cuadernos intercambian archivos, no variables de memoria. Pueden ejecutarse en sesiones separadas; no necesitan permanecer abiertos simultáneamente.

```text
01 · Arrestos ───────────────► arrestos_resumen.json ─────────────┐
02 · Vehículos ──────────────► vehiculos_resumen.json ────────────┤
03 · Pobreza y educación ────► pobreza_resumen.json ──────────────┼─► 04 · Integración
05 · Scraper NYCdata ────────► poblacion_nyc_2018.csv ─────────────┘

Bonos/01_Bono_NYHealth.ipynb ─► actividad independiente
Bonos/02_Bono_ClimaAPI.ipynb ─► actividad independiente: JSON, CSV y gráficas meteorológicas
```

Orden recomendado: **01 → 02 → 03 → 05 → 04**. Los tres primeros y el scraper no dependen entre sí; el cuaderno 4 sí requiere sus archivos de salida. Si cambia un dataset o su preparación, se debe regenerar el resumen correspondiente y volver a ejecutar la integración.

El CSV pequeño de población se lee con pandas y después se convierte en un DataFrame de Spark. Las tablas originales se procesan con Spark. Las celdas de cierre de los cuadernos principales liberan la caché y detienen su sesión; deben ejecutarse después de generar tablas y resultados.

## Archivos de resultados

| Grupo | Ejemplos | Contenido |
|---|---|---|
| Calidad | `01_calidad_arrestos.json`, `02_calidad_vehiculos.json`, `03_calidad_pobreza.json` | Conteos y porcentajes de faltantes por atributo. |
| Auditoría | `01_limpieza_arrestos.json`, `02_limpieza_vehiculos.json`, `03_limpieza_pobreza.json` | Trazabilidad de la depuración y controles específicos. |
| Conversión | `03_conversion_numerica.json` | Valores no vacíos que no pudieron convertirse a número. |
| Cobertura | `02_cobertura_arrestos.json`, `cobertura_vehiculos.json` | Distribución de registros por año. |
| Diccionarios | `diccionario_arrestos.json`, `diccionario_vehiculos.json`, `diccionario_pobreza.json` | Descripción de los atributos. |
| Descriptivos | `muestra_poblacion.json`, `edades.json`, `educacion.json`, `pobreza_ciudad.json`, `tipos_vehiculo.json` | Tablas e indicadores exploratorios. |
| Intercambio | `arrestos_resumen.json`, `pobreza_resumen.json`, `vehiculos_resumen.json`, `poblacion_nyc_2018.csv` | Entradas del cuaderno de integración. |
| Integración | `04_disponibilidad.json`, `04_base_territorial_preliminar.json` | Disponibilidad de datos y base territorial consolidada. |
| Visualizaciones | Archivos `.png` | Gráficas de calidad, arrestos, vehículos, edades y educación. |

La tabla incluye tanto archivos presentes como salidas previstas por el código. En esta copia del repositorio no están incluidos `vehiculos_resumen.json`, `unidades_vehiculos.json` ni `poblacion_nyc_2018.csv`; se generan al ejecutar los cuadernos 2 y 5 y deben estar disponibles en el entorno antes de integrar.

## Entorno y archivos necesarios para ejecutar

Los cuadernos principales utilizan Python 3.11, PySpark y Spark 4.2.0, además de pandas, matplotlib e IPython. Las extracciones de población utilizan requests y pandas; los dos cuadernos de `Bonos` también utilizan matplotlib. El bono meteorológico requiere una clave válida de OpenWeather y acceso a Internet, y no necesita iniciar una sesión de Spark. El kernel seleccionado debe disponer de las dependencias de cada cuaderno.

Los imports de los cuadernos requieren el módulo compartido `Utils.proyecto`, que centraliza el inicio de Spark, las rutas, la lectura, los diccionarios, la auditoría, las gráficas y la exportación. **La carpeta `Utils` no está incluida en esta copia del proyecto**: para reproducir la ejecución debe estar disponible en el entorno, junto con los archivos auxiliares que utiliza, como los diccionarios y la configuración del cluster. No debe confundirse con los diccionarios exportados dentro de `resultados`.

En el cluster se emplean estas ubicaciones:

```text
/opt/cluster/Proyecto/Entrega_1/datasets/
    Arrestos_NY.csv
    Accidentes_Viales_NY.csv
    Pobreza_NY.csv

/opt/cluster/Proyecto/Entrega_1/resultados/
    Resúmenes, reportes, gráficas y CSV de población
```

La carpeta `resultados` debe existir antes de ejecutar la celda de guardado del cuaderno 5. Los datos originales deben ser accesibles para los workers que los procesanm de tal forma que una ruta local no se vuelve compartida por estar dentro de una carpeta llamada cluster. Para trabajar desde diferentes servidores Jupyter, sus cuadernos deben acceder a los mismos archivos de resultados.

## Criterios para interpretar las salidas

- El reporte de faltantes describe los datos; no implica eliminar automáticamente toda fila incompleta ni imputar medias o medianas.
- Las exclusiones y los filtros deben interpretarse según el cálculo: una fila sin distrito no puede aportar a una agrupación territorial, aunque pueda servir para otro análisis.
- Los conteos de muestra y las sumas de pesos son magnitudes distintas. La suma de `PWGTP` estima población; no crea registros adicionales.
- El conteo de vehículos no equivale al conteo de colisiones, y el conteo de arrestos no equivale a personas únicas.
- Los resultados territoriales describen patrones agregados y no demuestran relaciones causales entre pobreza, educación y arrestos.

El repositorio conserva los cuadernos y las salidas compartidas como documentación del procedimiento. Volver a ejecutar el proyecto requiere los datasets originales, el módulo auxiliar y el entorno de procesamiento indicados anteriormente.
