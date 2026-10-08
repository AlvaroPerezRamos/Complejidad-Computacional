/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 3: Funciones primitivas recursivas de números naturales
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 08/10/2026
 * @file main.cc
 * @brief main TEMPORAL del paso 2 (funciones básicas). Comprueba Z, S y las
 * proyecciones Pᵢⁿ: su valor, su recuento de llamadas y sus errores. Se
 * sustituye en el paso 3.
 *
 * A diferencia del paso 1, ya no hacen falta funciones de juguete: se usan
 * las funciones básicas reales.
 */

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <utility>

#include "../include/call_counter.h"
#include "../include/errors.h"
#include "../include/projection_function.h"
#include "../include/successor_function.h"
#include "../include/zero_function.h"

namespace {

/** @brief Número de comprobaciones que han fallado; main() devuelve 1 si hay
 * alguna. */
int failed_checks = 0;

/** @brief Compara un valor obtenido con el esperado e imprime [OK] o [FALLO].
 */
void CheckEqual(const std::string& description,
                unsigned long long obtained_value,
                unsigned long long expected_value) {
  const bool passed = (obtained_value == expected_value);
  std::cout << (passed ? "  [OK]    " : "  [FALLO] ") << description
            << " -> obtenido " << obtained_value << ", esperado "
            << expected_value << "\n";
  if (!passed) ++failed_checks;
}

}  // namespace

int main() {
  CallCounter call_counter;

  // ---------------------------------------------------------------------------
  // Prueba 1: Z(x) = 0 para cualquier x.
  // ---------------------------------------------------------------------------
  std::cout << "1) Z(x) = 0, para cualquier x\n";
  const ZeroFunction zero_function;
  CheckEqual("Z(7)", zero_function.Evaluate({7}, call_counter), 0);
  CheckEqual("Z(0)", zero_function.Evaluate({0}, call_counter), 0);

  // ---------------------------------------------------------------------------
  // Prueba 2: S(x) = x + 1.
  // ---------------------------------------------------------------------------
  std::cout << "2) S(x) = x + 1\n";
  const SuccessorFunction successor_function;
  CheckEqual("S(0)", successor_function.Evaluate({0}, call_counter), 1);
  CheckEqual("S(4)", successor_function.Evaluate({4}, call_counter), 5);

  // ---------------------------------------------------------------------------
  // Prueba 3: Pᵢⁿ devuelve el argumento i-ésimo (i cuenta desde 1).
  // ---------------------------------------------------------------------------
  std::cout << "3) Pᵢⁿ devuelve el argumento i-ésimo (i cuenta desde 1)\n";
  const ProjectionFunction first_of_three(1, 3);
  const ProjectionFunction second_of_three(2, 3);
  const ProjectionFunction third_of_three(3, 3);
  CheckEqual("P_1^3(10, 20, 30)",
             first_of_three.Evaluate({10, 20, 30}, call_counter), 10);
  CheckEqual("P_2^3(10, 20, 30)",
             second_of_three.Evaluate({10, 20, 30}, call_counter), 20);
  CheckEqual("P_3^3(10, 20, 30)",
             third_of_three.Evaluate({10, 20, 30}, call_counter), 30);
  std::cout << "  nombre: " << second_of_three.GetName() << ", aridad "
            << second_of_three.GetArity() << "\n";

  // ---------------------------------------------------------------------------
  // Prueba 4: cada evaluación cuenta exactamente UNA llamada. Hasta aquí se han
  // hecho 2 (Z) + 2 (S) + 3 (P) = 7 evaluaciones con el mismo contador.
  // ---------------------------------------------------------------------------
  std::cout
      << "4) Cada evaluación cuenta exactamente una llamada (2 + 2 + 3 = 7)\n";
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 7);

  // ---------------------------------------------------------------------------
  // Prueba 5: una proyección mal construida se rechaza al construirla (no al
  // evaluarla): debe cumplirse 1 <= i <= n.
  // ---------------------------------------------------------------------------
  std::cout << "5) Proyección mal construida: debe cumplirse 1 <= i <= n\n";
  // P_0^3 (i demasiado pequeño) y P_4^3 (i mayor que n).
  const std::pair<std::size_t, std::size_t> invalid_projections[] = {{0, 3},
                                                                     {4, 3}};
  for (const std::pair<std::size_t, std::size_t>& invalid_projection :
       invalid_projections) {
    try {
      const ProjectionFunction projection(invalid_projection.first,
                                          invalid_projection.second);
      std::cout << "  [FALLO] P_" << invalid_projection.first << "^"
                << invalid_projection.second << " no lanzó excepción\n";
      ++failed_checks;
    } catch (const InvalidFunctionDefinitionError& error) {
      std::cout << "  [OK]    " << error.what() << "\n";
    }
  }

  // ---------------------------------------------------------------------------
  // Prueba 6: desbordamiento. S es el único punto donde un valor puede crecer,
  // así que es el único sitio donde se comprueba.
  // ---------------------------------------------------------------------------
  std::cout
      << "6) Desbordamiento: S(máximo natural) lanza NaturalOverflowError\n";
  const Natural largest_natural = std::numeric_limits<Natural>::max();
  CheckEqual("S(máximo - 1)",
             successor_function.Evaluate({largest_natural - 1}, call_counter),
             largest_natural);
  try {
    successor_function.Evaluate({largest_natural}, call_counter);
    std::cout << "  [FALLO] S(máximo) no lanzó excepción\n";
    ++failed_checks;
  } catch (const NaturalOverflowError& error) {
    std::cout << "  [OK]    " << error.what() << "\n";
  }

  std::cout << (failed_checks == 0 ? "\nTodo correcto.\n"
                                   : "\nHAY COMPROBACIONES FALLIDAS.\n");
  return failed_checks == 0 ? 0 : 1;
}