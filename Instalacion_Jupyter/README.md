# Tutorial: servidor Jupyter en una máquina virtual Linux

**Autor:** Gabriel Jaramillo Cuberos 
**Materia:** Procesamiento de Datos  
**Tema:** Instalación de Jupyter y acceso mediante SSH.

En este documento se explica cómo instalar JupyterLab en una máquina virtual Linux **Rocky** y acceder a sus cuadernos Python desde otro computador mediante un túnel SSH.

## 1. Objetivo

Instalar y ejecutar un servidor Jupyter en una máquina virtual Linux, acceder desde el navegador de otro computador y ejecutar un cuaderno Python.

Este procedimiento es independiente de Apache Hadoop y Spark.

## 2. Requisitos

- Una máquina virtual Linux encendida y conectada a la red.
- Acceso SSH a la máquina virtual.
- Python 3.11 y su módulo `venv`.
- Un navegador web.
- PowerShell o CMD con el comando `ssh` en el computador cliente.
- Acceso a internet para instalar JupyterLab.

> JupyterLab es una interfaz web que permite crear, editar y ejecutar cuadernos Jupyter Notebook (`.ipynb`).

### Valores utilizados en los ejemplos

Antes de ejecutar los comandos, reemplazar:

| Valor | Significado |
|---|---|
| `USUARIO_VM` | Usuario con acceso SSH a la máquina virtual. |
| `DIRECCION_VM` | Dirección IP o nombre de red de la máquina virtual. |
| `TOKEN_GENERADO` | Token que imprime Jupyter al iniciar el servidor. |

La dirección `127.0.0.1` representa al propio equipo y debe conservarse en los comandos indicados.

## 3. Conectarse a la máquina virtual

En Windows, abrir **PowerShell o CMD** y ejecutar:

```powershell
ssh USUARIO_VM@DIRECCION_VM
```

Introducir la contraseña cuando se solicite.

> Nota: al escribir la contraseña, SSH no muestra caracteres en pantalla. Es un protocolo de Linux.

Los siguientes comandos se ejecutan dentro de la sesión SSH, en la máquina virtual.

## 4. Verificar Python

Ejecutar:

```bash
python3.11 --version
```

El resultado debe indicar una versión de Python 3.11.

Si no está instalado, en distribuciones que utilizan `dnf` y tienen disponibles estos paquetes, ejecutar:

```bash
sudo dnf install python3.11 python3.11-pip
```

Después, verificar nuevamente:

```bash
python3.11 --version
```

> La instalación requiere permisos de administrador. En otras distribuciones, el gestor de paquetes y los comandos pueden ser diferentes.

## 5. Crear un entorno virtual

Ubicarse en la carpeta personal:

```bash
cd ~
```
o
```bash
cd
```

Crear el entorno:

```bash
python3.11 -m venv ~/jupyter-env
```

Activarlo:

```bash
source ~/jupyter-env/bin/activate
```

La terminal mostrará lo siguiente:

```text
(jupyter-env)
```

El entorno virtual permite instalar las dependencias de Jupyter en una carpeta propia.

> En este tutorial se utiliza `jupyter-env`. Si ya existe un entorno con Jupyter instalado, se puede utilizar su ruta en lugar de crear otro.

## 6. Instalar JupyterLab

Con el entorno activado, ejecutar:

```bash
python -m pip install jupyterlab
```

Esperar a que termine la instalación y verificar:

```bash
python --version
jupyter lab --version
```

Ambos comandos deben mostrar sus respectivas versiones.

> La creación del entorno y la instalación se realizan una sola vez. Para usos posteriores, basta con activar el entorno e iniciar Jupyter.

## 7. Iniciar el servidor Jupyter

En la terminal de la máquina virtual, ejecutar:

```bash
jupyter lab --ip=127.0.0.1 --port=8889 --no-browser
```

Las opciones utilizadas son:

| Opción | Función |
|---|---|
| `--ip=127.0.0.1` | Hace que Jupyter escuche únicamente en la interfaz local de la MV. |
| `--port=8889` | Solicita el puerto 8889 para el servidor web. |
| `--no-browser` | Evita intentar abrir un navegador dentro de la MV. |

La terminal mostrará un enlace parecido a:

```text
http://127.0.0.1:8889/lab?token=TOKEN_GENERADO
```

**Dejar esta terminal abierta.**

> Nota: el token permite acceder al servidor directamente desde el navegador.

Si Jupyter anuncia otro puerto porque el 8889 está ocupado, ajustar el túnel del siguiente paso al puerto anunciado. También se puede cerrar una instancia anterior que ya no se necesite y volver a iniciar Jupyter.

## 8. Crear el túnel SSH

Abrir **otra ventana de PowerShell o CMD en Windows** y ejecutar:

```powershell
ssh -L 8889:127.0.0.1:8889 USUARIO_VM@DIRECCION_VM
```

Introducir la contraseña y mantener esta conexión abierta.

El túnel permite acceder desde el navegador del computador cliente al servidor de la máquina virtual:

```text
Navegador en el computador cliente
        │
        ▼
Puerto local 8889
        │
        │ Túnel SSH
        ▼
Puerto 8889 de la máquina virtual
        │
        ▼
Servidor Jupyter
```

> Si este mismo túnel ya está abierto, no es necesario crear otro.

## 9. Abrir Jupyter en el navegador

En el navegador del computador cliente, abrir:

```text
http://127.0.0.1:8889/lab?token=TOKEN_GENERADO
```

Utilizar el token real que imprimió el servidor.

Aparecerá la interfaz de JupyterLab.

> Aunque el navegador está en el computador que accede a la máquina, el servidor y el código Python se ejecutan en la máquina virtual.

# Comprobación

Vamos a comprobar que la instalación se hizo correctamente.

## 10. Crear un cuaderno Python

En la pantalla inicial de JupyterLab:

1. Ubicar la sección **Notebook**.
2. Seleccionar **Python 3**, que puede aparecer como **Python 3 (ipykernel)**.
3. Renombrar el cuaderno como `Tutorial_Jupyter.ipynb`.

### Primera celda: comprobar Python

Escribir:

```python
import platform

print("Versión de Python:", platform.python_version())
print("¡Mi cuaderno Jupyter funciona!")
```

Ejecutar con **Shift + Enter**.

La salida mostrará la versión de Python y el mensaje:

```text
¡Mi cuaderno Jupyter funciona!
```

### Segunda celda: realizar un cálculo

Escribir:

```python
notas = [4.0, 4.5, 5.0]
promedio = sum(notas) / len(notas)

print("Notas:", notas)
print("Promedio:", promedio)
```

Ejecutar con **Shift + Enter**.

Resultado esperado:

```text
Notas: [4.0, 4.5, 5.0]
Promedio: 4.5
```

Guardar el cuaderno con **Ctrl + S**.

## 11. Comprobar el cumplimiento del objetivo

La demostración está completa cuando:

📌 El servidor Jupyter se inició desde la terminal de la MV.
📌 Se puede acceder a Jupyter desde el navegador.
📌 El cuaderno utiliza un kernel Python.
📌 La primera celda muestra la versión de Python.
📌 El cálculo devuelve el promedio `4.5`.
📌 El cuaderno queda guardado como archivo `.ipynb`.

## 12. Cerrar la sesión

Al terminar:

1. Guardar el cuaderno con **Ctrl + S**.
2. Ir a la terminal donde se inició Jupyter.
3. Pulsar **Ctrl + C** y confirmar el cierre si se solicita.
4. Cerrar cada conexión SSH escribiendo `exit`.
5. Cerrar la pestaña del navegador.

Cerrar únicamente el navegador no detiene necesariamente el servidor Jupyter.

## 13. Iniciar Jupyter en una próxima sesión

No es necesario reinstalar las herramientas.

### Conectarse desde PowerShell o CMD

```powershell
ssh USUARIO_VM@DIRECCION_VM
```

### Activar el entorno e iniciar Jupyter en la MV

```bash
source ~/jupyter-env/bin/activate
jupyter lab --ip=127.0.0.1 --port=8889 --no-browser
```

### Abrir el túnel en otra ventana de PowerShell o CMD

```powershell
ssh -L 8889:127.0.0.1:8889 USUARIO_VM@DIRECCION_VM
```

Después, abrir en el navegador el enlace con el token de la sesión actual y seleccionar el cuaderno guardado.

> Si cambia la dirección de la máquina virtual, actualizar `DIRECCION_VM` en los comandos SSH.

## 14. Problemas frecuentes

| Problema | Qué revisar |
|---|---|
| `jupyter: command not found` | Activar el entorno con `source ~/jupyter-env/bin/activate`. |
| El navegador no abre Jupyter | Comprobar que el servidor y el túnel SSH sigan abiertos y utilicen los puertos correctos. |
| El túnel indica que el puerto está ocupado | Revisar si ya existe un túnel o programa usando el puerto local 8889. |
| Jupyter solicita un token | Usar el token de la instancia actual, mostrado en la terminal del servidor. |
| SSH no logra conectarse | Verificar que la MV esté encendida y sea accesible desde la red del cliente. |
| El cuaderno intenta conectarse a Spark | Crear un cuaderno Python nuevo y utilizar las celdas de este tutorial, sin código de PySpark. |

## Referencia

- [Proyecto Jupyter: instalación](https://jupyter.org/install)