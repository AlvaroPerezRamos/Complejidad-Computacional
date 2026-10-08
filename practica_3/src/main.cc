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
 * @brief main TEMPORAL del paso 3 (composición). Comprueba Composition con
 * las funciones básicas: valor, número de llamadas y construcciones
 * inválidas. Se sustituye en el paso 4.
 *
 * Todavía no existe ninguna función de aridad 2 aparte de las proyecciones,
 * así que los ejemplos de varias funciones interiores usan proyecciones.
 */

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../include/basic_functions.h"
#include "../include/call_counter.h"
#include "../include/composition.h"
#include "../include/errors.h"

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

/**
 * @brief Comprueba que construir una composición inválida lanza
 * InvalidFunctionDefinitionError.
 * @param description Qué tiene de inválida la composición.
 * @param build_composition Código que intenta construirla.
 */
void CheckInvalidComposition(const std::string& description,
                             const std::function<void()>& build_composition) {
  try {
    build_composition();
    std::cout << "  [FALLO] " << description << " no lanzó excepción\n";
    ++failed_checks;
  } catch (const InvalidFunctionDefinitionError& error) {
    std::cout << "  [OK]    " << description << " -> " << error.what() << "\n";
  }
}

}  // namespace

int main() {
  const FunctionPointer zero_function = std::make_shared<const ZeroFunction>();
  const FunctionPointer successor_function =
      std::make_shared<const SuccessorFunction>();
  CallCounter call_counter;

  // ---------------------------------------------------------------------------
  // Prueba 1: uno = S ∘ Z, es decir uno(x) = S(Z(x)) = 1.
  // ---------------------------------------------------------------------------
  std::cout << "1) uno = S ∘ Z, es decir uno(x) = S(Z(x)) = 1\n";
  const FunctionPointer one_function = std::make_shared<const Composition>(
      successor_function, std::vector<FunctionPointer>{zero_function});
  CheckEqual("uno(9)", one_function->Evaluate({9}, call_counter), 1);
  // Llamadas: la composición (1) + Z (1) + S (1).
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 3);
  std::cout << "  nombre: " << one_function->GetName() << "\n";

  // ---------------------------------------------------------------------------
  // Prueba 2: una composición puede contener otra composición.
  // ---------------------------------------------------------------------------
  std::cout
      << "2) Composición de composiciones: dos = S ∘ uno = S(S(Z(x))) = 2\n";
  const FunctionPointer two_function = std::make_shared<const Composition>(
      successor_function, std::vector<FunctionPointer>{one_function});
  call_counter.Reset();
  CheckEqual("dos(9)", two_function->Evaluate({9}, call_counter), 2);
  // Llamadas: composición exterior (1) + uno (3) + S (1).
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 5);

  // ---------------------------------------------------------------------------
  // Prueba 3: varias funciones interiores. f recibe la TUPLA (g1(x), g2(x)),
  // no los argumentos originales.
  // ---------------------------------------------------------------------------
  std::cout
      << "3) Varias funciones interiores: P_1^2 ∘ (P_2^2, P_1^2) (x, y)\n";
  std::cout << "   La exterior recibe la tupla (y, x) y devuelve su primer "
               "valor: y\n";
  const FunctionPointer first_of_two =
      std::make_shared<const ProjectionFunction>(1, 2);
  const FunctionPointer second_of_two =
      std::make_shared<const ProjectionFunction>(2, 2);
  const Composition swapped_first(
      first_of_two, std::vector<FunctionPointer>{second_of_two, first_of_two});
  call_counter.Reset();
  CheckEqual("P_1^2 ∘ (P_2^2, P_1^2) (5, 8)",
             swapped_first.Evaluate({5, 8}, call_counter), 8);
  // Llamadas: composición (1) + dos interiores (2) + exterior (1).
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 4);
  CheckEqual("aridad de la composición (la de las interiores)",
             swapped_first.GetArity(), 2);

  // ---------------------------------------------------------------------------
  // Prueba 4: las composiciones incoherentes se rechazan AL CONSTRUIRLAS, no al
  // evaluarlas (mismo criterio que la proyección mal construida del paso 2).
  // ---------------------------------------------------------------------------
  std::cout << "4) Composiciones inválidas: se rechazan al construirlas\n";
  CheckInvalidComposition(
      "S ∘ (Z, Z): S tiene aridad 1 y se le dan 2 funciones", [&] {
        Composition(successor_function,
                    std::vector<FunctionPointer>{zero_function, zero_function});
      });
  CheckInvalidComposition(
      "P_1^2 ∘ (P_1^3, P_1^2): interiores de aridades distintas", [&] {
        const FunctionPointer first_of_three =
            std::make_shared<const ProjectionFunction>(1, 3);
        Composition(first_of_two,
                    std::vector<FunctionPointer>{first_of_three, first_of_two});
      });
  CheckInvalidComposition("sin funciones interiores", [&] {
    Composition(successor_function, std::vector<FunctionPointer>{});
  });
  CheckInvalidComposition("función exterior nula", [&] {
    Composition(nullptr, std::vector<FunctionPointer>{zero_function});
  });

  std::cout << (failed_checks == 0 ? "\nTodo correcto.\n"
                                   : "\nHAY COMPROBACIONES FALLIDAS.\n");
  return failed_checks == 0 ? 0 : 1;
}