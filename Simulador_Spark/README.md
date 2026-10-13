# SparkScope: simulador de Apache Spark

SparkScope es un simulador web interactivo que explica visualmente cómo se distribuye una carga de trabajo en un clúster Apache Spark y cómo este responde ante cambios de carga y fallos de workers.

El proyecto se implementa como un único archivo HTML. Funciona al abrirlo localmente en un navegador y puede desplegarse como sitio estático en Vercel.

## Objetivo pedagógico

La simulación representa conceptos fundamentales de Spark:

- Los usuarios generan jobs.
- Cada job se divide en tareas, que representan particiones lógicas de un RDD.
- El driver recibe los jobs y planifica las tareas.
- Los workers disponen de cores; cada core ejecuta una tarea a la vez.
- Si un worker falla, sus tareas activas regresan a la cola y se reasignan cuando hay capacidad disponible.

No se conecta a un clúster real. Las duraciones, la memoria y las métricas se generan con fines didácticos para permitir experimentar con los conceptos vistos en clase.

## Funcionalidades

| Elemento | Descripción |
| --- | --- |
| Usuarios concurrentes | Aumenta o reduce la frecuencia de generación de jobs. |
| Workers y cores | Modifica la capacidad de cómputo simulada. |
| Particiones por job | Controla la cantidad de tareas creadas para cada job. |
| Tasa de fallos | Define la probabilidad de fallos aleatorios. |
| Forzar fallo | Simula manualmente la caída de un worker activo. |
| Recuperación | Un worker caído vuelve a estar disponible después de varios ciclos. |
| Métricas | Muestra tareas terminadas, cola, utilización, latencia y fallos. |
| Gráfica | Muestra la evolución reciente de cola y utilización. |

## Planificador adaptativo

El modo adaptativo representa el componente inteligente del simulador. Para escoger el worker que recibirá la siguiente tarea, calcula una puntuación a partir de:

~~~text
puntuación =
  carga actual
  + duración promedio histórica de tareas
  + penalización por memoria ocupada
  + penalización por fallos recientes
~~~

El worker con menor puntuación recibe la tarea. Las métricas históricas se actualizan durante la simulación, por lo que la asignación se ajusta al comportamiento observado.

Al desactivar el modo adaptativo, cada tarea se asigna al primer worker con un core libre. Esto permite comparar una política sencilla de asignación con una política que considera métricas históricas.

## Cómo ejecutar localmente

1. Descargue o clone el repositorio.
2. Abra index.html con cualquier navegador moderno.
3. Ajuste los controles y observe los cambios en la cola, los workers y las métricas.

No requiere instalar Python, Node.js, Apache Spark ni dependencias adicionales.

## Despliegue en Vercel

1. Suba los cambios a GitHub.
2. En Vercel, use Add New Project e importe el repositorio.
3. En Root Directory, seleccione Simulador_Spark.
4. Seleccione el preset Other.
5. Deje vacíos Build Command y Output Directory.
6. Presione Deploy.

Vercel publicará index.html como página principal.

## Estructura

~~~text
Simulador_Spark/
├── index.html       Estructura y contenido de la página
├── css/
│   └── style.css    Estilos, colores, diseño y adaptación móvil
├── js/
│   └── app.js       Estado, simulación, planificador y gráfica
└── README.md        Documentación del proyecto
~~~

El código se separó intencionalmente para facilitar su lectura y mantenimiento:

- **index.html** contiene únicamente la estructura de la interfaz y las referencias a los archivos de estilos y lógica.
- **css/style.css** contiene los estilos visuales de los paneles, workers, métricas, botones y diseño responsivo.
- **js/app.js** contiene el comportamiento del simulador: generación de jobs, particiones, planificación, fallos, recuperación, métricas y gráfica.

## Referencia conceptual

La propuesta se basa en el modelo general de Spark: el driver coordina una aplicación y envía tareas a los ejecutores ubicados en workers. La interfaz simplifica este proceso para hacerlo visible durante una demostración.

Documentación oficial: https://spark.apache.org/docs/latest/cluster-overview.html
