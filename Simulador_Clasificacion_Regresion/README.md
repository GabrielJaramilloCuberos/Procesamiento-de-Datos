# MLlab: simulador de regresión logística y lineal

**Autor:** Gabriel Jaramillo Cuberos · **ID:** 20529022  
**Universidad:** Pontificia Universidad Javeriana  
**Curso:** Procesamiento de Datos (1255)  
**Fichero:** `Jaramillo_SimuladorMLlib.html`

## Cómo abrirlo

Abre `Jaramillo_SimuladorMLlib.html` con un navegador moderno. El simulador es un solo archivo HTML: no requiere instalación, servidor ni conexión a internet. La librería Three.js está incluida en el archivo.

## Cómo usar el simulador

1. Escoge **Regresión logística** para clasificar un caso como 0 o 1, o **Regresión lineal** para estimar un valor numérico.
2. Ajusta el número de registros, el ruido, la semilla y la proporción de datos reservada para entrenamiento.
3. Pulsa **Entrenar** para avanzar por el flujo de aprendizaje, o **Paso** para observar cada etapa y avanzar una iteración a la vez.
4. Revisa las métricas y las gráficas. Puedes pausar, continuar y reiniciar el entrenamiento.
5. Modifica workers, cores, particiones o activa **skew** para comparar la carga y los tiempos estimados.
6. Prueba valores nuevos en el laboratorio de predicción, o abre **Datos propios** para importar un CSV.

En la escena 3D puedes arrastrar para girar, usar los botones para acercar y alejar, y restablecer la vista. También puedes enfocar la escena y usar las flechas del teclado.

## Qué aprenderás

- **Regresión logística:** estima la probabilidad de una clase binaria con la función sigmoide. El umbral cambia la clasificación; métricas como exactitud, precisión, recall y F1 ayudan a evaluar el resultado.
- **Regresión lineal:** estima un valor continuo mediante una combinación ponderada de características. RMSE, MAE y R² describen el error en datos reservados.
- **Entrenamiento:** los pesos se actualizan para reducir una función de pérdida. La regularización L1 y L2 limita la magnitud de los coeficientes y puede reducir el sobreajuste.
- **Procesamiento distribuido:** las particiones calculan gradientes parciales que se agregan en un modelo global. La visualización tridimensional muestra datos, workers, driver y flujo de gradientes.
- **Evaluación:** una parte de los datos se reserva para medir el desempeño en ejemplos que no participaron en el ajuste.

El HTML incluye explicaciones conceptuales, fórmulas, instrucciones paso a paso, escenarios de experimentación y un ejemplo de código PySpark.

## Configuración de datos

El simulador genera ejemplos sintéticos a partir de dos características: horas de estudio (`x1`) y prácticas realizadas (`x2`). Usa una semilla para repetir un experimento con los mismos datos. Los controles de workers, cores y particiones modifican la distribución y la estimación del tiempo, sin borrar los pesos aprendidos.

Para importar un archivo, usa CSV numérico con esta cabecera exacta:

```csv
x1,x2,label
2,3,0
8,9,1
```

Admite de 20 a 10.000 filas, separadas por comas, con punto decimal y sin comillas. Para regresión logística, `label` debe ser 0 o 1 y cada clase necesita al menos dos filas. El tamaño máximo del archivo es 2 MB. Los datos se procesan en el navegador. El simulador también permite descargar los datos actuales y exportar un informe JSON.

## Cómo interpretar la simulación de carga

Los tiempos son estimaciones didácticas con un modelo de coste fijo; no son mediciones de un clúster ni pruebas de rendimiento de Apache Spark. Al activar **skew**, una partición recibe una fracción mayor de los registros para mostrar cómo una tarea desbalanceada puede prolongar el trabajo. Cambiar la velocidad solo cambia la animación.

Los cálculos y el entrenamiento ocurren localmente y de forma secuencial en el navegador. La vista de workers explica el procesamiento distribuido; el HTML no se conecta ni ejecuta Spark real.

## Archivos

| Archivo | Uso |
|---|---|
| `Jaramillo_SimuladorMLlib.html` | Simulador interactivo autocontenido. Este es el archivo que se entrega. |
| `README.md` | Guía del proyecto. |

## Fuentes

- [Documentación de clasificación y regresión de Spark MLlib](https://spark.apache.org/docs/latest/ml-classification-regression.html)
- [API de regresión logística en PySpark](https://spark.apache.org/docs/latest/api/python/reference/api/pyspark.ml.classification.LogisticRegression.html)
- [API de regresión lineal en PySpark](https://spark.apache.org/docs/latest/api/python/reference/api/pyspark.ml.regression.LinearRegression.html)
- [Documentación de Three.js](https://threejs.org/docs/)

Three.js se distribuye bajo licencia MIT, incluida en el HTML. Simulador desarrollado con apoyo de IA. El diseño toma como referencia el simulador MapReduce compartido por Salomon Alfredo Avila Larrotta; el código de este proyecto es una implementación propia.
