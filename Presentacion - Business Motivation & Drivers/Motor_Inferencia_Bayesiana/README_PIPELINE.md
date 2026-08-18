# Pipeline: Transacciones → Red Bayesiana → Decisión de negocio

## Qué hace cada pieza

```
transacciones.csv (se genera solo la primera vez)
        ↓
procesamiento.py   → calcula compras_totales, gasto_total, ticket_promedio
                    → aplica reglas de negocio -> FRECUENCIA (baja/media/alta), VALOR (bajo/alto)
                    → observa RECOMPRA (si/no) en la ventana posterior al corte
                    → cuenta y aprende P(FRECUENCIA), P(VALOR), P(RECOMPRA|FRECUENCIA,VALOR)
                    → escribe variables.txt, probabilidades.txt, consulta.txt, resumen_consulta.txt
        ↓
lanza.pl            → corre procesamiento.py
                    → verifica que los 4 archivos existan
                    → compila el motor C++ si no existe ./motor
                    → automatiza el menú del motor (opciones 1, 2, 4, 0) por stdin
                    → imprime la probabilidad de recompra interpretada
        ↓
motor (C++)          → RedBayesiana + VariableAleatoria + inferencia por enumeración (sin cambios internos)
```

## Dónde va cada archivo

Todo debe quedar en la **misma carpeta** que ya tienes (`Proyecto_3_Intro_a_la_IA/`), junto a `main.cpp`, `TADS/`, `Utils/`:

```
Proyecto_3_Intro_a_la_IA/
├── main.cpp
├── TADS/...
├── Utils/...
├── procesamiento.py      <- nuevo
├── lanza.pl               <- nuevo
├── transacciones.csv      <- se genera solo, no lo edites a mano
├── variables.txt           <- se sobreescribe automáticamente
├── probabilidades.txt      <- se sobreescribe automáticamente
├── consulta.txt             <- se sobreescribe automáticamente
└── resumen_consulta.txt     <- se sobreescribe automáticamente
```

Los `variables.txt`/`probabilidades.txt` de ejemplo (RAIN/TRAIN...) se sobreescriben con
la red nueva (FRECUENCIA/VALOR → RECOMPRA). Si quieres conservar el ejemplo original,
haz una copia (`cp variables.txt variables_rain_demo.txt`) antes de correr el pipeline.

## Requisitos

- Python 3 con `pandas` y `numpy`:
  ```bash
  pip install pandas numpy --break-system-packages
  ```
- `g++` y `perl` (ya los usa tu proyecto).

## Cómo correr todo

```bash
cd Proyecto_3_Intro_a_la_IA
perl lanza.pl
```

Eso solo. `lanza.pl` compila el motor si hace falta, corre Python, y ejecuta la inferencia.

Si ya tienes el ejecutable compilado con otro nombre/ruta:
```bash
perl lanza.pl ./ruta/al/ejecutable
```

## Compilar el motor C++ manualmente (opcional)

`lanza.pl` lo hace solo si no existe `./motor`, pero si quieres hacerlo a mano:
```bash
g++ main.cpp TADS/VariableAleatoria.cpp TADS/RedBayesiana.cpp Utils/utils.cpp -o motor
```

## Qué red bayesiana se construye

```
FRECUENCIA (baja/media/alta) ─┐
                                ├──> RECOMPRA (si/no)
VALOR (bajo/alto) ─────────────┘
```

- **FRECUENCIA** y **VALOR** son variables raíz: sus probabilidades salen de contar
  cuántos clientes caen en cada categoría, del total de clientes.
- **RECOMPRA** es la variable dependiente: su CPT sale de contar, para cada combinación
  observada de (FRECUENCIA, VALOR), cuántos clientes recompraron o no en la ventana
  posterior al corte. Ninguna probabilidad se escribe a mano.
- Si alguna combinación (FRECUENCIA, VALOR) no tiene observaciones (o tiene pocas), se
  aplica **Laplace smoothing (add-one)**: `P = (conteo + 1) / (total_combinación + 2)`.
  Esto evita divisiones por cero y garantiza que el archivo de probabilidades siempre
  sea válido, sin inventar probabilidades fuera de los datos.
- La regla de negocio para frecuencia/valor y la etiqueta RECOMPRA nunca se mezclan:
  RECOMPRA se observa en una ventana de 30 días **después** del corte, así que no hay
  circularidad con la frecuencia histórica.

## Ejemplo de salida final

```
Cliente 140
  - compras: 5
  - gasto: $528.37
  - frecuencia: alta
  - valor: alto
  - probabilidad de recompra (si): 0.875
  - probabilidad de recompra (no): 0.125
```

Interpretación para la presentación:
"Los datos transaccionales permiten construir variables de comportamiento (frecuencia,
valor). Estas variables alimentan un modelo probabilístico aprendido a partir del
histórico (Red Bayesiana), cuya inferencia por enumeración exacta permite estimar la
probabilidad de recompra futura y apoyar una decisión empresarial (por ejemplo, a qué
clientes priorizar en una campaña de retención)."
