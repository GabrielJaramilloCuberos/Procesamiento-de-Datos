#!/usr/bin/perl
# ============================================================
# lanza.pl
# ------------------------------------------------------------
# Orquestador del pipeline:
#   transacciones -> procesamiento.py -> variables.txt / probabilidades.txt
#                  -> motor C++ (RedBayesiana) -> inferencia por enumeracion
#
# Uso:
#   perl lanza.pl                 (usa ./motor por defecto)
#   perl lanza.pl ./ruta/al/motor (indica el ejecutable C++)
# ============================================================
use strict;
use warnings;

my $EJECUTABLE = $ARGV[0] // "./motor";

print "======================================================\n";
print " PIPELINE: Datos -> Informacion -> Inferencia -> Decision\n";
print "======================================================\n\n";

# ------------------------------------------------------------
# Paso 1: ejecutar el procesamiento en Python
# ------------------------------------------------------------
print ">> [1/4] Ejecutando procesamiento.py ...\n\n";
my $status_py = system("python3", "procesamiento.py");
if ($status_py != 0) {
    die "\nERROR: procesamiento.py fallo (codigo $status_py). Se detiene el pipeline.\n";
}

# ------------------------------------------------------------
# Paso 2: verificar que los archivos esperados existan
# ------------------------------------------------------------
print "\n>> [2/4] Verificando archivos generados ...\n";
for my $archivo (qw(variables.txt probabilidades.txt consulta.txt resumen_consulta.txt)) {
    die "ERROR: no se encontro '$archivo'. Revisa procesamiento.py.\n" unless -e $archivo;
    print "   OK  $archivo\n";
}

# ------------------------------------------------------------
# Paso 3: compilar el motor C++ si hace falta
# ------------------------------------------------------------
if (!-x $EJECUTABLE) {
    print "\n>> Ejecutable '$EJECUTABLE' no encontrado. Compilando...\n";
    my $status_cc = system(
        "g++ main.cpp TADS/VariableAleatoria.cpp TADS/RedBayesiana.cpp Utils/utils.cpp -o motor"
    );
    die "ERROR: fallo la compilacion del motor C++.\n" if $status_cc != 0;
    $EJECUTABLE = "./motor";
    print "   Compilado como ./motor\n";
}

# ------------------------------------------------------------
# Paso 4: leer la consulta generada por Python y automatizar el menu del motor
# ------------------------------------------------------------
open(my $fh_consulta, "<", "consulta.txt") or die "No se pudo leer consulta.txt: $!\n";
my $consulta = <$fh_consulta>;
chomp $consulta;
close $fh_consulta;

my %resumen;
open(my $fh_resumen, "<", "resumen_consulta.txt") or die "No se pudo leer resumen_consulta.txt: $!\n";
while (my $linea = <$fh_resumen>) {
    chomp $linea;
    my ($clave, $valor) = split(/=/, $linea, 2);
    $resumen{$clave} = $valor if defined $clave && defined $valor;
}
close $fh_resumen;

print "\n>> [3/4] Ejecutando el motor de inferencia ($EJECUTABLE) ...\n";
print "   Consulta automatizada: $consulta\n\n";

# El motor es interactivo (menu por stdin). Se automatiza asi:
#   1            -> cargar variables
#   variables    -> nombre del archivo (sin .txt)
#   2            -> cargar probabilidades
#   probabilidades -> nombre del archivo (sin .txt)
#   4            -> consulta por enumeracion
#   <consulta>   -> la consulta generada por Python
#   0            -> salir
my $entrada = "1\nvariables\n2\nprobabilidades\n4\n$consulta\n0\n";

open(my $motor, "|-", "$EJECUTABLE > salida_motor.txt") or die "No se pudo ejecutar $EJECUTABLE: $!\n";
print $motor $entrada;
close $motor;

open(my $fh_salida, "<", "salida_motor.txt") or die "No se pudo leer la salida del motor: $!\n";
my @lineas_salida = <$fh_salida>;
close $fh_salida;

print @lineas_salida;

# ------------------------------------------------------------
# Paso 5: extraer y presentar el resultado final de forma legible
# ------------------------------------------------------------
print "\n>> [4/4] Resultado interpretado para decision de negocio\n";
print "======================================================\n";
printf "Cliente %s\n", $resumen{CLIENTE_ID} // "?";
printf "  - compras: %s\n", $resumen{COMPRAS} // "?";
printf "  - gasto: \$%s\n", $resumen{GASTO} // "?";
printf "  - frecuencia: %s\n", $resumen{FRECUENCIA} // "?";
printf "  - valor: %s\n", $resumen{VALOR} // "?";

# El texto "P(RECOMPRA=x | evidencia) = y" aparece dos veces en la salida del
# motor (una vez en la traza de normalizacion y otra en el resumen final),
# asi que se deduplica quedandonos con el ultimo valor visto por cada valor.
my %resultado;
my @orden_valores;
for my $linea (@lineas_salida) {
    if ($linea =~ /P\(RECOMPRA=(\w+)\s*\|\s*evidencia\)\s*=\s*([\d.]+)/) {
        push @orden_valores, $1 unless exists $resultado{$1};
        $resultado{$1} = $2;
    }
}
for my $valor (@orden_valores) {
    printf "  - probabilidad de recompra (%s): %s\n", $valor, $resultado{$valor};
}
print "======================================================\n";
