#!/bin/bash
# Universidad de La Laguna - Complejidad Computacional - Práctica 1
#
# Recorre toda la batería de errores y avisos de la sección 6 de
# AutomataPila.md (33 filas en total) y comprueba que cada fichero de
# test/errores/, test/avisos/ y test/cadenas_erroneas/ dispara exactamente
# lo que le corresponde, con el programa ya compilado en build/bin/.
#
# Uso: ./test/run_error_and_warning_tests.sh [ruta_al_binario]

set -u

BINARY="${1:-build/bin/automata_pila}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PASS_COUNT=0
FAIL_COUNT=0

if [ ! -x "$BINARY" ]; then
  echo "No se encuentra el ejecutable en '$BINARY'. Compila el proyecto primero."
  exit 1
fi

# check_case <descripcion> <patron_esperado_en_la_salida> <codigo_salida_esperado> -- <argumentos...>
check_case() {
  local description="$1"; shift
  local expected_pattern="$1"; shift
  local expected_exit_code="$1"; shift
  shift  # descarta el separador "--"

  local output
  output=$("$@" < /dev/null 2>&1)
  local actual_exit_code=$?

  if echo "$output" | grep -qF "$expected_pattern" && [ "$actual_exit_code" -eq "$expected_exit_code" ]; then
    echo "  [OK] $description"
    PASS_COUNT=$((PASS_COUNT + 1))
  else
    echo "  [FALLO] $description"
    echo "          esperado: patrón '$expected_pattern', exit $expected_exit_code"
    echo "          obtenido: exit $actual_exit_code"
    echo "$output" | sed 's/^/          > /'
    FAIL_COUNT=$((FAIL_COUNT + 1))
  fi
}

echo "=== Errores de línea de comandos ==="
cd "$SCRIPT_DIR/.."
check_case "falta -trace" "hacen falta '-config' y '-trace'" 1 -- "$BINARY" -config test/APf/APf-1.txt
check_case "opción repetida" "está repetida" 1 -- "$BINARY" -config test/APf/APf-1.txt -trace n -config test/APf/APf-1.txt
check_case "opción sin valor" "le falta el valor" 1 -- "$BINARY" -config
check_case "valor de -trace inválido" "debe ser 'y' o 'n'" 1 -- "$BINARY" -config test/APf/APf-1.txt -trace x
check_case "opción desconocida" "Opción desconocida" 1 -- "$BINARY" -config test/APf/APf-1.txt -trace n -foo bar
check_case "-config inaccesible" "No se puede acceder al fichero de '-config'" 1 -- "$BINARY" -config no_existe.txt -trace n
check_case "-in inaccesible" "No se puede acceder al fichero de '-in'" 1 -- "$BINARY" -config test/APf/APf-1.txt -trace n -in no_existe.txt
check_case "-out junto a -trace n" "el fichero de traza quedaría vacío" 1 -- "$BINARY" -config test/APf/APf-1.txt -trace n -out /tmp/traza_test.txt
check_case "-out coincide con -config" "se destruiría" 1 -- "$BINARY" -config test/APf/APf-1.txt -trace y -out test/APf/APf-1.txt
check_case "-h muestra ayuda" "Uso:" 0 -- "$BINARY" -h

echo ""
echo "=== Errores del fichero de configuración ==="
check_case "fichero vacío" "está vacío" 1 -- "$BINARY" -config test/errores/fichero_vacio.txt -trace n
check_case "fichero truncado" "incompleto" 1 -- "$BINARY" -config test/errores/fichero_truncado.txt -trace n
check_case "formato APv (sin línea de F)" "autómata por vaciado de pila" 1 -- "$BINARY" -config test/errores/formato_apv.txt -trace n
check_case "estado repetido en Q" "aparece repetido" 1 -- "$BINARY" -config test/errores/estado_duplicado.txt -trace n
check_case "símbolo repetido en Σ" "aparece repetido" 1 -- "$BINARY" -config test/errores/simbolo_duplicado_sigma.txt -trace n
check_case "símbolo de más de un carácter" "exactamente un carácter" 1 -- "$BINARY" -config test/errores/simbolo_multicaracter.txt -trace n
check_case "'.' declarado en un alfabeto" "reservado" 1 -- "$BINARY" -config test/errores/epsilon_en_alfabeto.txt -trace n
check_case "más de un estado inicial" "exactamente un estado" 1 -- "$BINARY" -config test/errores/varios_estados_iniciales.txt -trace n
check_case "q0 no pertenece a Q" "no pertenece a Q" 1 -- "$BINARY" -config test/errores/estado_inicial_no_declarado.txt -trace n
check_case "estado de F no pertenece a Q" "no pertenece a Q" 1 -- "$BINARY" -config test/errores/estado_final_no_declarado.txt -trace n
check_case "Z0 no pertenece a Γ" "no pertenece a Γ" 1 -- "$BINARY" -config test/errores/z0_no_declarado.txt -trace n
check_case "transición con campos de más/menos" "exactamente 5 campos" 1 -- "$BINARY" -config test/errores/transicion_campos.txt -trace n
check_case "origen de transición no declarado" "no está declarado en Q" 1 -- "$BINARY" -config test/errores/origen_no_declarado.txt -trace n
check_case "destino de transición no declarado" "no está declarado en Q" 1 -- "$BINARY" -config test/errores/destino_no_declarado.txt -trace n
check_case "símbolo de entrada ajeno a Σ∪{ε}" "Σ ∪ {ε}" 1 -- "$BINARY" -config test/errores/simbolo_entrada_no_declarado.txt -trace n
check_case "cima consultada ajena a Γ" "no pertenece a Γ" 1 -- "$BINARY" -config test/errores/cima_no_declarada.txt -trace n
check_case "cima consultada es ε" "nunca puede consultar ε" 1 -- "$BINARY" -config test/errores/cima_es_epsilon.txt -trace n
check_case "símbolo a apilar ajeno a Γ" "no pertenece a Γ" 1 -- "$BINARY" -config test/errores/apilar_simbolo_no_declarado.txt -trace n
check_case "mezcla de '.' al apilar" "no puede mezclar" 1 -- "$BINARY" -config test/errores/apilar_mezcla_epsilon.txt -trace n

echo ""
echo "=== Avisos (no abortan) ==="
check_case "transición duplicada" "Transición duplicada" 0 -- "$BINARY" -config test/avisos/transicion_duplicada.txt -trace n
check_case "autómata sin transiciones" "no tiene ninguna transición" 0 -- "$BINARY" -config test/avisos/sin_transiciones.txt -trace n
check_case "estado inalcanzable" "es inalcanzable desde q0" 0 -- "$BINARY" -config test/avisos/estado_inalcanzable.txt -trace n
check_case "ningún estado final alcanzable" "lenguaje reconocido es vacío" 0 -- "$BINARY" -config test/avisos/ningun_final_alcanzable.txt -trace n
check_case "Σ ∩ Γ ≠ ∅" "comparten" 0 -- "$BINARY" -config test/avisos/interseccion_alfabetos.txt -trace n
check_case "sin transición aplicable en (q0,Z0)" "se detiene nada más arrancar" 0 -- "$BINARY" -config test/avisos/sin_transicion_inicial.txt -trace n

echo ""
echo "=== Errores de las cadenas de entrada ==="
check_case "símbolo de la cadena ajeno a Σ" "no pertenece al alfabeto de entrada" 0 -- \
  "$BINARY" -config test/cadenas_erroneas/anbn.txt -trace n -in test/cadenas_erroneas/simbolo_fuera_sigma.txt
check_case "límite de exploración superado" "Se ha superado el tamaño máximo de la pila" 0 -- \
  "$BINARY" -config test/cadenas_erroneas/ciclo_creciente.txt -trace n -in test/cadenas_erroneas/limite_superado.txt

echo ""
echo "================================================================"
echo "Resultado: $PASS_COUNT correctos, $FAIL_COUNT fallidos (de $((PASS_COUNT + FAIL_COUNT)))"
echo "================================================================"

[ "$FAIL_COUNT" -eq 0 ]
