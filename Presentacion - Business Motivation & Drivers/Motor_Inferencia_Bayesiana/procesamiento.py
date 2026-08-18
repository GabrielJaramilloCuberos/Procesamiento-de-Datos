"""
procesamiento.py
-----------------
Pipeline: transacciones -> variables de negocio -> probabilidades (Red Bayesiana).

Genera automaticamente:
  - variables.txt        (estructura de la red: FRECUENCIA->RECOMPRA, VALOR->RECOMPRA)
  - probabilidades.txt   (CPTs aprendidas de los datos, formato compatible con RedBayesiana.cpp)
  - consulta.txt          (consulta lista para el motor C++, formato "P(VAR | EVID=val,...)")
  - resumen_consulta.txt  (contexto legible: que cliente / segmento se va a consultar)

Ninguna probabilidad se escribe a mano: todo sale de conteos sobre transacciones.csv
(que se genera una sola vez si no existe, para que la demo sea reproducible).
"""

import os
import numpy as np
import pandas as pd
from datetime import datetime, timedelta

RNG_SEED = 42
ARCHIVO_TRANSACCIONES = "transacciones.csv"

# Fecha de corte: separa historico (para calcular variables) de la ventana de observacion
FECHA_CORTE = datetime(2024, 1, 31)
VENTANA_DIAS = 30
INICIO_HISTORICO = FECHA_CORTE - timedelta(days=120)
FIN_VENTANA = FECHA_CORTE + timedelta(days=VENTANA_DIAS)

VALORES_FRECUENCIA = ["baja", "media", "alta"]
VALORES_VALOR = ["bajo", "alto"]
VALORES_RECOMPRA = ["si", "no"]


# ============================================================
# 1. DATOS: cargar si ya existen, o simular un dataset de demo
# ============================================================

def generar_transacciones_demo(n_clientes=40, seed=RNG_SEED):
    """Simula transacciones de clientes en el periodo historico + ventana.

    La probabilidad de recomprar en la ventana se muestrea de forma
    estocastica (con una tendencia realista: clientes mas activos en el
    historico tienden a ser mas leales), pero NUNCA se deriva con una
    formula fija a partir de la frecuencia. Es una observacion posterior
    e independiente, no un calculo circular.
    """
    rng = np.random.default_rng(seed)
    filas = []

    for id_cliente in range(101, 101 + n_clientes):
        # Numero de compras historicas por cliente (variable entre clientes)
        n_compras = rng.choice([1, 2, 3, 4, 5, 6], p=[0.20, 0.25, 0.20, 0.15, 0.12, 0.08])
        dias_compra = rng.integers(0, (FECHA_CORTE - INICIO_HISTORICO).days, size=n_compras)
        montos = rng.uniform(15, 140, size=n_compras).round(2)

        for dia, monto in zip(sorted(dias_compra), montos):
            fecha = INICIO_HISTORICO + timedelta(days=int(dia))
            filas.append((id_cliente, fecha, float(monto)))

        # Propension oculta a recomprar: mas compras y mayor gasto -> mas probable
        # (esto simula comportamiento real, pero la etiqueta se OBSERVA, no se copia)
        gasto_total = montos.sum()
        propension = 0.15 + 0.10 * min(n_compras, 5) + 0.001 * gasto_total
        propension = min(propension, 0.9)

        if rng.random() < propension:
            dia_recompra = rng.integers(1, VENTANA_DIAS)
            fecha_recompra = FECHA_CORTE + timedelta(days=int(dia_recompra))
            monto_recompra = round(float(rng.uniform(15, 140)), 2)
            filas.append((id_cliente, fecha_recompra, monto_recompra))

    df = pd.DataFrame(filas, columns=["id_cliente", "fecha", "monto_compra"])
    df = df.sort_values(["id_cliente", "fecha"]).reset_index(drop=True)
    return df


def cargar_o_crear_transacciones():
    if os.path.exists(ARCHIVO_TRANSACCIONES):
        df = pd.read_csv(ARCHIVO_TRANSACCIONES, parse_dates=["fecha"])
        print(f"Transacciones cargadas desde {ARCHIVO_TRANSACCIONES} ({len(df)} filas).")
    else:
        df = generar_transacciones_demo()
        df.to_csv(ARCHIVO_TRANSACCIONES, index=False)
        print(f"Dataset de demo generado y guardado en {ARCHIVO_TRANSACCIONES} ({len(df)} filas).")
    return df


# ============================================================
# 2. REGLAS DE NEGOCIO: numeros -> categorias
# ============================================================

def clasificar_frecuencia(compras_totales):
    if compras_totales <= 1:
        return "baja"
    elif compras_totales <= 3:
        return "media"
    else:
        return "alta"


def clasificar_valor(gasto_total):
    return "bajo" if gasto_total < 100 else "alto"


# ============================================================
# 3. CONSTRUCCION DEL PERFIL DE CLIENTE (historico + etiqueta RECOMPRA)
# ============================================================

def construir_perfil(df):
    historico = df[df["fecha"] <= FECHA_CORTE]
    ventana = df[(df["fecha"] > FECHA_CORTE) & (df["fecha"] <= FIN_VENTANA)]

    perfil = historico.groupby("id_cliente").agg(
        compras_totales=("monto_compra", "count"),
        gasto_total=("monto_compra", "sum"),
        ticket_promedio=("monto_compra", "mean"),
    ).reset_index()

    perfil["frecuencia"] = perfil["compras_totales"].apply(clasificar_frecuencia)
    perfil["valor"] = perfil["gasto_total"].apply(clasificar_valor)

    # RECOMPRA se observa despues del corte, NUNCA se deriva de frecuencia/valor
    clientes_que_recompraron = set(ventana["id_cliente"].unique())
    perfil["recompra"] = perfil["id_cliente"].apply(
        lambda cid: "si" if cid in clientes_que_recompraron else "no"
    )

    return perfil


# ============================================================
# 4. APRENDIZAJE DE PROBABILIDADES (conteos -> CPTs)
# ============================================================

def aprender_probabilidades(perfil):
    n = len(perfil)

    # P(FRECUENCIA) y P(VALOR): frecuencias relativas directas (siempre definidas)
    p_frecuencia = {v: (perfil["frecuencia"] == v).sum() / n for v in VALORES_FRECUENCIA}
    p_valor = {v: (perfil["valor"] == v).sum() / n for v in VALORES_VALOR}

    # P(RECOMPRA | FRECUENCIA, VALOR): con Laplace smoothing (add-one)
    # para que ninguna combinacion (frecuencia, valor) sin observaciones
    # quede en division por cero o ausente del archivo.
    k = len(VALORES_RECOMPRA)  # 2
    p_recompra = {}
    for f in VALORES_FRECUENCIA:
        for v in VALORES_VALOR:
            subset = perfil[(perfil["frecuencia"] == f) & (perfil["valor"] == v)]
            total_fv = len(subset)
            for r in VALORES_RECOMPRA:
                cnt = (subset["recompra"] == r).sum()
                p = (cnt + 1) / (total_fv + k)  # Laplace smoothing
                p_recompra[(f, v, r)] = p

    return p_frecuencia, p_valor, p_recompra


# ============================================================
# 5. GENERACION DE ARCHIVOS PARA EL MOTOR C++
# ============================================================

def escribir_variables_txt(ruta="variables.txt"):
    with open(ruta, "w", newline="\n") as f:
        f.write("FRECUENCIA RECOMPRA\n")
        f.write("VALOR RECOMPRA\n")


def escribir_probabilidades_txt(p_frecuencia, p_valor, p_recompra, ruta="probabilidades.txt"):
    with open(ruta, "w", newline="\n") as f:
        f.write("FRECUENCIA\n")
        for v in VALORES_FRECUENCIA:
            f.write(f"{v} {p_frecuencia[v]:.6f}\n")
        f.write("\n")

        f.write("VALOR\n")
        for v in VALORES_VALOR:
            f.write(f"{v} {p_valor[v]:.6f}\n")
        f.write("\n")

        # El motor normaliza la condicion ordenando alfabeticamente sus componentes:
        # FRECUENCIA < VALOR alfabeticamente, por eso el encabezado y cada linea
        # de datos usan ese mismo orden.
        f.write("RECOMPRA | FRECUENCIA,VALOR\n")
        for fr in VALORES_FRECUENCIA:
            for va in VALORES_VALOR:
                for r in VALORES_RECOMPRA:
                    prob = p_recompra[(fr, va, r)]
                    f.write(f"FRECUENCIA={fr},VALOR={va} {r} {prob:.6f}\n")


def escribir_consulta(perfil, ruta_consulta="consulta.txt", ruta_resumen="resumen_consulta.txt"):
    """Elige un cliente representativo (frecuencia alta + valor alto si existe,
    si no el de mayor gasto) y escribe la consulta para el motor C++ + un
    resumen legible que usara lanza.pl para presentar el resultado."""
    candidatos = perfil[(perfil["frecuencia"] == "alta") & (perfil["valor"] == "alto")]
    if len(candidatos) > 0:
        cliente = candidatos.sort_values("gasto_total", ascending=False).iloc[0]
    else:
        cliente = perfil.sort_values("gasto_total", ascending=False).iloc[0]

    frecuencia = cliente["frecuencia"]
    valor = cliente["valor"]

    consulta = f"P(RECOMPRA | FRECUENCIA={frecuencia},VALOR={valor})"
    with open(ruta_consulta, "w", newline="\n") as f:
        f.write(consulta + "\n")

    with open(ruta_resumen, "w", newline="\n") as f:
        f.write(f"CLIENTE_ID={int(cliente['id_cliente'])}\n")
        f.write(f"COMPRAS={int(cliente['compras_totales'])}\n")
        f.write(f"GASTO={cliente['gasto_total']:.2f}\n")
        f.write(f"TICKET_PROMEDIO={cliente['ticket_promedio']:.2f}\n")
        f.write(f"FRECUENCIA={frecuencia}\n")
        f.write(f"VALOR={valor}\n")
        f.write(f"RECOMPRA_OBSERVADA={cliente['recompra']}\n")

    return cliente, consulta


# ============================================================
# 6. MAIN
# ============================================================

def main():
    df = cargar_o_crear_transacciones()

    print("\n=== VENTANAS ===")
    print(f"Historico : hasta {FECHA_CORTE.date()}")
    print(f"Ventana   : {FECHA_CORTE.date()} -> {FIN_VENTANA.date()} ({VENTANA_DIAS} dias)")

    perfil = construir_perfil(df)

    print("\n=== PERFIL DE CLIENTES (primeras filas) ===")
    print(perfil.head(10).to_string(index=False))

    print("\n=== METRICAS DE NEGOCIO ===")
    print(f"Clientes analizados       : {len(perfil)}")
    print(f"Transacciones historicas  : {(df['fecha'] <= FECHA_CORTE).sum()}")
    print(f"Clientes que recompraron  : {(perfil['recompra'] == 'si').sum()} "
          f"({(perfil['recompra'] == 'si').mean() * 100:.1f}%)")
    print("Distribucion FRECUENCIA   :", perfil["frecuencia"].value_counts().to_dict())
    print("Distribucion VALOR        :", perfil["valor"].value_counts().to_dict())

    p_frecuencia, p_valor, p_recompra = aprender_probabilidades(perfil)

    print("\n=== PROBABILIDADES APRENDIDAS ===")
    print("P(FRECUENCIA):")
    for v in VALORES_FRECUENCIA:
        print(f"  P(FRECUENCIA={v}) = {p_frecuencia[v]:.4f}")
    print("P(VALOR):")
    for v in VALORES_VALOR:
        print(f"  P(VALOR={v}) = {p_valor[v]:.4f}")
    print("P(RECOMPRA | FRECUENCIA, VALOR)  [con Laplace smoothing add-1]:")
    for fr in VALORES_FRECUENCIA:
        for va in VALORES_VALOR:
            print(f"  P(RECOMPRA=si | FRECUENCIA={fr},VALOR={va}) = "
                  f"{p_recompra[(fr, va, 'si')]:.4f}")

    escribir_variables_txt()
    escribir_probabilidades_txt(p_frecuencia, p_valor, p_recompra)
    cliente, consulta = escribir_consulta(perfil)

    print("\n=== ARCHIVOS GENERADOS ===")
    print("variables.txt, probabilidades.txt, consulta.txt, resumen_consulta.txt")

    print("\n=== CLIENTE ELEGIDO PARA LA CONSULTA DE INFERENCIA ===")
    print(f"Cliente {int(cliente['id_cliente'])}")
    print(f"  compras          : {int(cliente['compras_totales'])}")
    print(f"  gasto            : ${cliente['gasto_total']:.2f}")
    print(f"  frecuencia       : {cliente['frecuencia']}")
    print(f"  valor            : {cliente['valor']}")
    print(f"  recompra real    : {cliente['recompra']}")
    print(f"  consulta al motor: {consulta}")


if __name__ == "__main__":
    main()
