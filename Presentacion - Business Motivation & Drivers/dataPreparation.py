import pandas as pd

# ============================================================
# 1. SIMULACIÓN DE DATOS DE TRANSACCIONES
# ============================================================

datos = {
    'id_cliente': [101, 102, 101, 103, 101, 102],
    'monto_compra': [50.0, 75.0, 45.0, 120.0, 30.0, 80.0]
}

df = pd.DataFrame(datos)

print("=== DATOS DE TRANSACCIONES ===")
print(df)


# ============================================================
# 2. ANÁLISIS DEL COMPORTAMIENTO DE LOS CLIENTES
# ============================================================

analisis = df.groupby('id_cliente').agg(
    compras_totales=('monto_compra', 'count'),
    gasto_total=('monto_compra', 'sum'),
    ticket_promedio=('monto_compra', 'mean')
).reset_index()


# ============================================================
# 3. SEGMENTACIÓN POR FRECUENCIA DE COMPRA
# ============================================================

analisis['es_recurrente'] = (
    analisis['compras_totales'] > 1
)


# ============================================================
# 4. SEGMENTACIÓN POR VALOR DEL CLIENTE
# ============================================================

analisis['segmento_valor'] = pd.cut(
    analisis['gasto_total'],
    bins=[0, 100, 200, float('inf')],
    labels=['Bajo', 'Medio', 'Alto']
)


# ============================================================
# 5. PARTICIPACIÓN DEL CLIENTE EN LAS VENTAS
# ============================================================

ventas_totales = analisis['gasto_total'].sum()

analisis['participacion_ventas'] = (
    analisis['gasto_total'] / ventas_totales * 100
).round(2)


# ============================================================
# 6. RESULTADO DEL ANÁLISIS
# ============================================================

print("\n=== PERFIL DE CLIENTES ===")
print(analisis)


# ============================================================
# 7. IDENTIFICAR CLIENTES RECURRENTES Y DE ALTO VALOR
# ============================================================

clientes_prioritarios = analisis[
    (analisis['es_recurrente']) &
    (analisis['segmento_valor'] == 'Alto')
]

print("\n=== CLIENTES PRIORITARIOS ===")
print(clientes_prioritarios)


# ============================================================
# 8. MÉTRICAS DE NEGOCIO
# ============================================================

print("\n=== MÉTRICAS DEL NEGOCIO ===")

print(
    "Ventas totales: $",
    round(df['monto_compra'].sum(), 2)
)

print(
    "Número de clientes:",
    df['id_cliente'].nunique()
)

print(
    "Número de transacciones:",
    len(df)
)

print(
    "Clientes recurrentes:",
    analisis['es_recurrente'].sum()
)

print(
    "Porcentaje de clientes recurrentes:",
    round(
        analisis['es_recurrente'].mean() * 100,
        2
    ),
    "%"
)