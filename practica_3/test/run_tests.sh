#!/bin/bash
# Universidad de La Laguna - Complejidad Computacional - Práctica 3
#
# Pruebas de la Práctica 3 (potencia como función primitiva recursiva), con el
# programa ya compilado en build/bin/:
#   - resultado y número de llamadas de potencia(x, y), contrastados con un
#     evaluador independiente (recursión literal que cuenta cada invocación);
#   - todos los errores de la línea de comandos, con su código de salida;
#   - las pruebas unitarias (build/bin/pruebas_unitarias), que cubren lo que la
#     línea de comandos no alcanza: definiciones inválidas, desbordamiento y
#     aridades.
#
# Los valores esperados NO salen del propio programa.
#
# Uso: ./test/run_tests.sh [ruta_al_binario]

set -u

BINARY="${1:-build/bin/potencia}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PASS_COUNT=0
FAIL_COUNT=0

cd "$SCRIPT_DIR/.."

if [ ! -x "$BINARY" ]; then
  echo "No se encuentra el ejecutable en '$BINARY'. Compila el proyecto primero."
  exit 1
fi

UNIT_TESTS_BINARY="$(dirname "$BINARY")/pruebas_unitarias"

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

# check_power <x> <y> <resultado_esperado> <llamadas_esperadas>
# La salida debe ser EXACTAMENTE las dos líneas esperadas y el exit 0.
check_power() {
  local base="$1" exponent="$2" expected_result="$3" expected_calls="$4"
  local expected_output actual_output actual_exit_code
  expected_output="potencia($base, $exponent) = $expected_result
Número de llamadas a funciones: $expected_calls"
  actual_output=$("$BINARY" -x "$base" -y "$exponent" < /dev/null 2>&1)
  actual_exit_code=$?

  if [ "$actual_exit_code" -eq 0 ] && [ "$actual_output" == "$expected_output" ]; then
    report_pass "potencia($base, $exponent) = $expected_result con $expected_calls llamadas"
  else
    report_fail "potencia($base, $exponent)" \
      "$(echo "$expected_output" | tr '\n' '|'), exit 0" "$actual_exit_code" "$actual_output"
  fi
}

# check_same_output <descripcion> <argumentos_a> -- <argumentos_b>
# Las dos invocaciones deben producir la misma salida (con exit 0).
check_same_output() {
  local description="$1"; shift
  local arguments_a=() arguments_b=()
  while [ "$1" != "--" ]; do arguments_a+=("$1"); shift; done
  shift  # descarta "--"
  arguments_b=("$@")

  local output_a output_b
  output_a=$("$BINARY" "${arguments_a[@]}" < /dev/null 2>&1) && \
    output_b=$("$BINARY" "${arguments_b[@]}" < /dev/null 2>&1)
  if [ $? -eq 0 ] && [ "$output_a" == "$output_b" ]; then
    report_pass "$description"
  else
    report_fail "$description" "la misma salida con ${arguments_a[*]} y con ${arguments_b[*]}" "?" \
      "$(printf 'A: %s\nB: %s' "$output_a" "$output_b")"
  fi
}


echo "=== Resultado y número de llamadas ==="
check_power 2 3 8 114
check_power 3 2 9 100
check_power 5 0 1 4        # y = 0: la propia potencia (1) + uno(x) (3)
check_power 0 0 1 4        # convención 0^0 = 1
check_power 0 4 0 28
check_power 3 1 3 40
check_power 1 5 1 84
check_power 7 2 49 324
check_power 2 10 1024 8368
check_power 10 4 10000 44708
check_power 3 8 6561 39556
check_power 2 20 1048576 8388964   # antes no terminaba en un tiempo razonable

echo ""
echo "=== Límites del tipo Natural ==="
check_power 18446744073709551615 0 1 4   # el mayor Natural como base, sin desbordar
check_power 1000 1 1000 10010            # coste lineal en el valor del resultado

echo ""
echo "=== Orden de las opciones y formato de los valores ==="
check_same_output "-x y -y en cualquier orden" -x 7 -y 2 -- -y 2 -x 7
check_same_output "ceros a la izquierda (007 = 7)" -x 007 -y 2 -- -x 7 -y 2

echo ""
echo "=== Errores de línea de comandos (exit 1, con la ayuda) ==="
check_case "sin argumentos: falta -x" "Falta la opción obligatoria '-x'" 1 -- "$BINARY"
check_case "falta -y" "Falta la opción obligatoria '-y'" 1 -- "$BINARY" -x 2
check_case "falta -x" "Falta la opción obligatoria '-x'" 1 -- "$BINARY" -y 3
check_case "-x repetida" "La opción '-x' está repetida" 1 -- "$BINARY" -x 2 -y 3 -x 4
check_case "-y repetida" "La opción '-y' está repetida" 1 -- "$BINARY" -x 2 -y 3 -y 4
check_case "repetida: se detecta antes de convertir el valor" "La opción '-x' está repetida" 1 -- "$BINARY" -x 2 -y 3 -x abc
check_case "-x sin valor" "A la opción '-x' le falta el valor" 1 -- "$BINARY" -x
check_case "-y sin valor" "A la opción '-y' le falta el valor" 1 -- "$BINARY" -x 2 -y
check_case "opción desconocida" "Opción desconocida: '-z'" 1 -- "$BINARY" -x 2 -y 3 -z 1
check_case "argumento suelto" "Opción desconocida: 'foo'" 1 -- "$BINARY" foo
check_case "valor negativo" "se ha recibido '-1'" 1 -- "$BINARY" -x -1 -y 3
check_case "valor con signo +" "se ha recibido '+5'" 1 -- "$BINARY" -x +5 -y 3
check_case "valor decimal" "se ha recibido '2.5'" 1 -- "$BINARY" -x 2.5 -y 3
check_case "valor con letras" "se ha recibido '12abc'" 1 -- "$BINARY" -x 12abc -y 3
check_case "valor vacío" "se ha recibido ''" 1 -- "$BINARY" -x "" -y 3
check_case "valor que no cabe en 64 bits" "no cabe en un natural de 64 bits" 1 -- "$BINARY" -x 18446744073709551616 -y 1
check_case "los errores incluyen la ayuda" "Uso:" 1 -- "$BINARY" -x 2

echo ""
echo "=== Ayuda ==="
check_case "-h muestra la ayuda" "Uso:" 0 -- "$BINARY" -h
check_case "--help muestra la ayuda" "Uso:" 0 -- "$BINARY" --help
check_case "-h tiene prioridad sobre un error posterior" "Uso:" 0 -- "$BINARY" -h -z

echo ""
echo "=== Pruebas unitarias (pruebas_unitarias) ==="
if [ ! -x "$UNIT_TESTS_BINARY" ]; then
  report_fail "pruebas unitarias" "ejecutable en '$UNIT_TESTS_BINARY'" "-" "Compila el proyecto primero."
else
  unit_output=$("$UNIT_TESTS_BINARY" 2>&1)
  unit_exit_code=$?
  if [ "$unit_exit_code" -eq 0 ] && echo "$unit_output" | grep -qF "Todo correcto."; then
    report_pass "todas las pruebas unitarias ($(echo "$unit_output" | grep -c '\[OK\]') comprobaciones)"
  else
    report_fail "pruebas unitarias" "exit 0 y 'Todo correcto.'" "$unit_exit_code" "$(echo "$unit_output" | grep 'FALLO')"
  fi
fi

echo ""
echo "Resultado: $PASS_COUNT correctas, $FAIL_COUNT fallidas."
[ "$FAIL_COUNT" -eq 0 ]