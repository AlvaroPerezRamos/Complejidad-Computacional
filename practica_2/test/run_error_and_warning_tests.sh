#!/bin/bash
# Universidad de La Laguna - Complejidad Computacional - Práctica 2
#
# Recorre la batería de errores y avisos de la sección 6 de MaquinaTuring.md
# y comprueba que cada fichero de test/errores/, test/avisos/,
# test/cadenas_erroneas/ y test/cadena_vacia/ dispara exactamente lo que le
# corresponde, con el programa ya compilado en build/bin/.
#
# Más estricto que el de la Práctica 1:
#   - los errores de configuración se comprueban CON su número de línea real;
#   - los avisos se cuentan: cada fichero debe emitir exactamente los suyos
#     (ni uno menos, ni uno de más);
#   - las cuatro MT válidas del repositorio no deben emitir ningún aviso.
#
# Uso: ./test/run_error_and_warning_tests.sh [ruta_al_binario]

set -u

BINARY="${1:-build/bin/maquina_turing}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PASS_COUNT=0
FAIL_COUNT=0

if [ ! -x "$BINARY" ] && [ ! -x "$SCRIPT_DIR/../$BINARY" ]; then
  echo "No se encuentra el ejecutable en '$BINARY'. Compila el proyecto primero."
  exit 1
fi

cd "$SCRIPT_DIR/.."

report_pass() {
  echo "  [OK] $1"
  PASS_COUNT=$((PASS_COUNT + 1))
}

report_fail() {
  echo "  [FALLO] $1"
  echo "          esperado: $2"
  echo "          obtenido: exit $3"
  echo "$4" | sed 's/^/          > /'
  FAIL_COUNT=$((FAIL_COUNT + 1))
}

# check_case <descripcion> <patron> <exit_esperado> -- <comando...>
# La salida (stdout+stderr) debe contener el patrón literal y el programa
# debe terminar con el código de salida esperado.
check_case() {
  local description="$1"; shift
  local expected_pattern="$1"; shift
  local expected_exit_code="$1"; shift
  shift  # descarta "--"

  local output actual_exit_code
  output=$("$@" < /dev/null 2>&1)
  actual_exit_code=$?

  if echo "$output" | grep -qF -- "$expected_pattern" && [ "$actual_exit_code" -eq "$expected_exit_code" ]; then
    report_pass "$description"
  else
    report_fail "$description" "patrón '$expected_pattern', exit $expected_exit_code" "$actual_exit_code" "$output"
  fi
}

# check_count <descripcion> <patron> <n_esperado> <exit_esperado> -- <comando...>
# El patrón literal debe aparecer en EXACTAMENTE n líneas de la salida.
check_count() {
  local description="$1"; shift
  local pattern="$1"; shift
  local expected_count="$1"; shift
  local expected_exit_code="$1"; shift
  shift  # descarta "--"

  local output actual_exit_code actual_count
  output=$("$@" < /dev/null 2>&1)
  actual_exit_code=$?
  actual_count=$(echo "$output" | grep -cF -- "$pattern")

  if [ "$actual_count" -eq "$expected_count" ] && [ "$actual_exit_code" -eq "$expected_exit_code" ]; then
    report_pass "$description"
  else
    report_fail "$description" "'$pattern' en $expected_count línea(s), exit $expected_exit_code" \
      "$actual_exit_code (aparece en $actual_count línea(s))" "$output"
  fi
}


echo "=== Errores de línea de comandos ==="
check_case "falta -config" "Falta la opción obligatoria '-config'" 1 -- "$BINARY"
check_case "-config repetida" "La opción '-config' está repetida" 1 -- "$BINARY" -config test/Ejemplo1_MT.txt -config test/Ejemplo1_MT.txt
check_case "-in repetida" "La opción '-in' está repetida" 1 -- "$BINARY" -config test/Ejemplo1_MT.txt -in a.txt -in b.txt
check_case "opción sin valor" "le falta el valor" 1 -- "$BINARY" -config
check_case "opción desconocida" "Opción desconocida: '-foo'" 1 -- "$BINARY" -config test/Ejemplo1_MT.txt -foo bar
check_case "-trace no existe en P02" "Opción desconocida: '-trace'" 1 -- "$BINARY" -config test/Ejemplo1_MT.txt -trace y
check_case "-config inaccesible" "No se puede acceder al fichero de '-config'" 1 -- "$BINARY" -config no_existe.txt
check_case "-in inaccesible" "No se puede acceder al fichero de '-in'" 1 -- "$BINARY" -config test/Ejemplo1_MT.txt -in no_existe.txt
check_case "-h muestra la ayuda" "Uso:" 0 -- "$BINARY" -h
check_case "--help muestra la ayuda" "Uso:" 0 -- "$BINARY" --help

echo ""
echo "=== Errores del fichero de configuración (con su número de línea) ==="
E=test/errores
check_case "fichero vacío" "está vacío" 1 -- "$BINARY" -config $E/fichero_vacio.txt
check_case "termina antes de Σ" "(línea -1): El fichero de configuración está incompleto: falta la línea de Σ" 1 -- "$BINARY" -config $E/fichero_truncado_sin_sigma.txt
check_case "termina antes de Γ" "(línea -1): El fichero de configuración está incompleto: falta la línea de Γ" 1 -- "$BINARY" -config $E/fichero_truncado_sin_gamma.txt
check_case "termina antes de q0" "(línea -1): El fichero de configuración está incompleto: falta la línea de q0" 1 -- "$BINARY" -config $E/fichero_truncado_sin_estado_inicial.txt
check_case "termina antes del blanco" "(línea -1): El fichero de configuración está incompleto: falta la línea del símbolo blanco" 1 -- "$BINARY" -config $E/fichero_truncado_sin_blanco.txt
check_case "termina antes de F" "(línea -1): El fichero de configuración está incompleto: falta la línea de F" 1 -- "$BINARY" -config $E/fichero_truncado_sin_finales.txt
check_case "termina antes del nº de cintas" "(línea -1): El fichero de configuración está incompleto: falta la línea del número de cintas" 1 -- "$BINARY" -config $E/fichero_truncado_sin_numero_cintas.txt
check_case "estado repetido en Q" "(línea 2): El estado 'q0' aparece repetido en Q" 1 -- "$BINARY" -config $E/estado_duplicado_q.txt
check_case "estado repetido en F" "(línea 7): El estado 'q1' aparece repetido en F" 1 -- "$BINARY" -config $E/estado_duplicado_f.txt
check_case "símbolo repetido en Σ" "(línea 3): El símbolo '0' aparece repetido en el alfabeto" 1 -- "$BINARY" -config $E/simbolo_duplicado_sigma.txt
check_case "símbolo repetido en Γ" "(línea 4): El símbolo '0' aparece repetido en el alfabeto" 1 -- "$BINARY" -config $E/simbolo_duplicado_gamma.txt
check_case "símbolo de más de un carácter en Σ" "(línea 3): El símbolo '11' no es válido" 1 -- "$BINARY" -config $E/simbolo_multicaracter_sigma.txt
check_case "símbolo de más de un carácter en Γ" "(línea 4): El símbolo 'ab' no es válido" 1 -- "$BINARY" -config $E/simbolo_multicaracter_gamma.txt
check_case "blanco de más de un carácter" "(línea 6): El símbolo blanco '..' no es válido" 1 -- "$BINARY" -config $E/blanco_multicaracter.txt
check_case "línea del blanco con varios tokens" "(línea 6): El símbolo blanco debe ser exactamente un símbolo" 1 -- "$BINARY" -config $E/blanco_varios_tokens.txt
check_case "el blanco no pertenece a Γ" "(línea 6): El símbolo blanco '.' no pertenece a Γ" 1 -- "$BINARY" -config $E/blanco_no_en_gamma.txt
check_case "el blanco pertenece a Σ" "(línea 6): El símbolo blanco '.' no puede pertenecer a Σ" 1 -- "$BINARY" -config $E/blanco_en_sigma.txt
check_case "más de un estado inicial" "(línea 5): El estado inicial (q0) debe ser exactamente un estado" 1 -- "$BINARY" -config $E/varios_estados_iniciales.txt
check_case "q0 no pertenece a Q" "(línea 5): El estado inicial 'q2' (q0) no pertenece a Q" 1 -- "$BINARY" -config $E/estado_inicial_no_declarado.txt
check_case "estado de F no pertenece a Q" "(línea 7): El estado final 'q2' no pertenece a Q" 1 -- "$BINARY" -config $E/estado_final_no_declarado.txt
check_case "nº de cintas = 0" "(línea 8): El número de cintas debe ser mayor que 0" 1 -- "$BINARY" -config $E/cintas_cero.txt
check_case "nº de cintas no numérico" "(línea 8): El número de cintas 'dos' no es un entero positivo válido" 1 -- "$BINARY" -config $E/cintas_no_numerico.txt
check_case "nº de cintas negativo" "(línea 8): El número de cintas '-1' no es un entero positivo válido" 1 -- "$BINARY" -config $E/cintas_negativo.txt
check_case "nº de cintas decimal" "(línea 8): El número de cintas '1.5' no es un entero positivo válido" 1 -- "$BINARY" -config $E/cintas_decimal.txt
check_case "nº de cintas desbordado" "(línea 8): El número de cintas '99999999999999999999' es demasiado grande" 1 -- "$BINARY" -config $E/cintas_desbordado.txt
check_case "nº de cintas con varios valores" "(línea 8): El número de cintas debe ser un único valor" 1 -- "$BINARY" -config $E/cintas_varios_valores.txt
check_case "transición sin movimiento (la pista lo deduce)" "(línea 9): La transición 'q0 0 q1 0' no es válida: se esperaban 5 campos (origen, 1 símbolo(s) leído(s), destino, 1 símbolo(s) a escribir, 1 movimiento(s)); tiene 4. Parece que falta el movimiento." 1 -- "$BINARY" -config $E/transicion_sin_movimiento.txt
check_case "transición sin estado de origen" "(línea 9): La transición '0 q1 0 R' no es válida: se esperaban 5 campos" 1 -- "$BINARY" -config $E/transicion_sin_origen.txt
check_case "...la pista dice que falta el origen" "Parece que falta el estado de origen." 1 -- "$BINARY" -config $E/transicion_sin_origen.txt
check_case "transición sin símbolo leído: pista" "Parece que falta el símbolo leído." 1 -- "$BINARY" -config $E/transicion_sin_simbolo_leido.txt
check_case "transición sin estado de destino: pista" "Parece que falta el estado de destino." 1 -- "$BINARY" -config $E/transicion_sin_destino.txt
check_case "transición sin símbolo a escribir: pista" "Parece que falta el símbolo a escribir." 1 -- "$BINARY" -config $E/transicion_sin_simbolo_escrito.txt
check_case "transición con un campo de más: pista" "Parece que sobra el campo '0'." 1 -- "$BINARY" -config $E/transicion_campo_de_mas.txt
check_case "N=2 sin estado de destino: pista" "(línea 9): La transición 'q0 0 . 0 . R S' no es válida: se esperaban 8 campos" 1 -- "$BINARY" -config $E/transicion_n2_sin_destino.txt
check_case "...y dice que falta el destino" "Parece que falta el estado de destino." 1 -- "$BINARY" -config $E/transicion_n2_sin_destino.txt
check_case "N=2 sin símbolo leído: pista (de alguna cinta)" "Parece que falta el símbolo leído de alguna de las cintas." 1 -- "$BINARY" -config $E/transicion_n2_sin_simbolo_leido.txt
check_case "N=2 sin símbolo a escribir: pista (de alguna cinta)" "Parece que falta el símbolo a escribir de alguna de las cintas." 1 -- "$BINARY" -config $E/transicion_n2_sin_simbolo_escrito.txt
check_case "N=2 sin movimiento: pista (de alguna cinta)" "Parece que falta el movimiento de alguna de las cintas." 1 -- "$BINARY" -config $E/transicion_n2_sin_movimiento.txt
check_case "campos agrupados por cinta en vez de por tipo" "(línea 10): El estado de destino 'R' de la transición 'q0 0 0 R . . S q1' no está declarado en Q" 1 -- "$BINARY" -config $E/transicion_orden_por_cinta.txt
check_case "transición de 1 cinta en una MT de 2 (N=2)" "(línea 9): La transición 'q0 0 q1 0 R' no es válida: se esperaban 8 campos" 1 -- "$BINARY" -config $E/transicion_campos_n2.txt
check_case "origen de transición no declarado" "(línea 9): El estado de origen 'q2'" 1 -- "$BINARY" -config $E/transicion_origen_no_declarado.txt
check_case "destino de transición no declarado" "(línea 9): El estado de destino 'q2'" 1 -- "$BINARY" -config $E/transicion_destino_no_declarado.txt
check_case "símbolo leído fuera de Γ" "(línea 9): El símbolo leído '2' de la transición 'q0 2 q1 0 R' no pertenece a Γ" 1 -- "$BINARY" -config $E/transicion_simbolo_leido_fuera_gamma.txt
check_case "símbolo escrito fuera de Γ" "(línea 9): El símbolo a escribir '2' de la transición 'q0 0 q1 2 R' no pertenece a Γ" 1 -- "$BINARY" -config $E/transicion_simbolo_escrito_fuera_gamma.txt
check_case "símbolo leído de más de un carácter" "(línea 9): El símbolo leído '00' de la transición 'q0 00 q1 0 R' no es válido" 1 -- "$BINARY" -config $E/transicion_simbolo_multicaracter.txt
check_case "movimiento ajeno a {L,R,S}" "(línea 9): El movimiento 'X' de la transición 'q0 0 q1 0 X' no es válido" 1 -- "$BINARY" -config $E/transicion_movimiento_invalido.txt
check_case "movimiento en minúscula" "(línea 9): El movimiento 'r' de la transición 'q0 0 q1 0 r' no es válido" 1 -- "$BINARY" -config $E/transicion_movimiento_minuscula.txt
check_case "MT no determinista (1 cinta)" "(línea 10): Ya existe una transición distinta" 1 -- "$BINARY" -config $E/no_determinista_n1.txt
check_case "MT no determinista (2 cintas)" "(línea 10): Ya existe una transición distinta" 1 -- "$BINARY" -config $E/no_determinista_n2.txt

echo ""
echo "=== Avisos (no abortan: exit 0) ==="
A=test/avisos
check_case  "transición duplicada: mensaje" "Transición duplicada, se ignora" 0 -- "$BINARY" -config $A/transicion_duplicada.txt
check_count "transición duplicada: exactamente 1 aviso" "[Aviso]" 1 0 -- "$BINARY" -config $A/transicion_duplicada.txt
check_case  "sin transiciones: mensaje" "no tiene ninguna transición" 0 -- "$BINARY" -config $A/sin_transiciones.txt
check_count "sin transiciones: exactamente 1 aviso" "[Aviso]" 1 0 -- "$BINARY" -config $A/sin_transiciones.txt
check_case  "estado inalcanzable: mensaje" "El estado 'q_isla' es inalcanzable desde q0" 0 -- "$BINARY" -config $A/estado_inalcanzable.txt
check_count "estado inalcanzable: exactamente 1 aviso" "[Aviso]" 1 0 -- "$BINARY" -config $A/estado_inalcanzable.txt
check_case  "ningún final alcanzable: mensaje" "Ningún estado final es alcanzable desde q0" 0 -- "$BINARY" -config $A/ningun_final_alcanzable.txt
check_count "ningún final alcanzable: exactamente 2 avisos (q2 inalcanzable + ningún final)" "[Aviso]" 2 0 -- "$BINARY" -config $A/ningun_final_alcanzable.txt

echo ""
echo "=== Las MT válidas no emiten ningún aviso (detecta falsos positivos del validador) ==="
for machine in Ejemplo1_MT Ejemplo2_MT Problema1_anbm Problema2_conteo; do
  check_count "$machine: 0 avisos" "[Aviso]" 0 0 -- "$BINARY" -config test/$machine.txt
done

echo ""
echo "=== Errores de las cadenas de entrada (no abortan: se continúa con la siguiente) ==="
check_case "símbolo de la cadena ajeno a Σ" "La cadena '02' contiene el símbolo '2', que no pertenece al alfabeto de entrada" 0 -- \
  "$BINARY" -config test/Ejemplo1_MT.txt -in test/cadenas_erroneas/simbolo_fuera_sigma.txt
check_case "...y la cadena siguiente sí se procesa" "RECHAZADA  00[.]" 0 -- \
  "$BINARY" -config test/Ejemplo1_MT.txt -in test/cadenas_erroneas/simbolo_fuera_sigma.txt

echo ""
echo "=== Convenio de la cadena vacía ==="
check_case "'.' (el blanco) es la cadena vacía" "RECHAZADA  [.]" 0 -- \
  "$BINARY" -config test/Ejemplo1_MT.txt -in test/cadena_vacia/vacia_con_blanco.txt
check_case "una línea vacía es la cadena vacía" "RECHAZADA  [.]" 0 -- \
  "$BINARY" -config test/Ejemplo1_MT.txt -in test/cadena_vacia/vacia_linea_vacia.txt
check_case "con blanco '_': '_' es la cadena vacía" "ACEPTADA  [_]" 0 -- \
  "$BINARY" -config test/cadena_vacia/blanco_guion_config.txt -in test/cadena_vacia/blanco_guion_in.txt
check_case "con blanco '_': '.' ya no lo es (ajena a Σ)" "La cadena '.' contiene el símbolo '.'" 0 -- \
  "$BINARY" -config test/cadena_vacia/blanco_guion_config.txt -in test/cadena_vacia/blanco_guion_in.txt

echo ""
echo "================================================================"
echo "Resultado: $PASS_COUNT correctos, $FAIL_COUNT fallidos (de $((PASS_COUNT + FAIL_COUNT)))"
echo "================================================================"

[ "$FAIL_COUNT" -eq 0 ]
