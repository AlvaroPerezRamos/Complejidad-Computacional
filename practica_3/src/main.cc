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
 * @brief main TEMPORAL del paso 4 (recursión primitiva). Comprueba
 * PrimitiveRecursion construyendo a mano la suma, sin librería de funciones:
 *
 *     suma(x, 0)    = x                 = P_1^1(x)
 *     suma(x, S(y)) = S(suma(x, y))     = (S ∘ P_3^3)(x, y, suma(x, y))
 *
 * Se sustituye en el paso 5, donde la suma pasa a la librería de funciones.
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
#include "../include/primitive_recursion.h"

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
 * @brief Comprueba que construir una recursión inválida lanza
 * InvalidFunctionDefinitionError.
 * @param description Qué tiene de inválida la recursión.
 * @param build_recursion Código que intenta construirla.
 */
void CheckInvalidRecursion(const std::string& description,
                           const std::function<void()>& build_recursion) {
  try {
    build_recursion();
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
  const FunctionPointer first_of_one =
      std::make_shared<const ProjectionFunction>(1, 1);
  const FunctionPointer second_of_three =
      std::make_shared<const ProjectionFunction>(2, 3);
  const FunctionPointer third_of_three =
      std::make_shared<const ProjectionFunction>(3, 3);
  CallCounter call_counter;

  // ---------------------------------------------------------------------------
  // Prueba 1: la suma, construida a mano con g = P_1^1 y h = S ∘ P_3^3.
  // ---------------------------------------------------------------------------
  std::cout << "1) suma = Rec[P_1^1, S ∘ P_3^3]\n";
  const FunctionPointer successor_of_accumulated =
      std::make_shared<const Composition>(
          successor_function, std::vector<FunctionPointer>{third_of_three});
  const FunctionPointer addition_function =
      std::make_shared<const PrimitiveRecursion>(first_of_one,
                                                 successor_of_accumulated);
  std::cout << "  nombre: " << addition_function->GetName() << ", aridad "
            << addition_function->GetArity() << "\n";
  CheckEqual("aridad de la suma (la de g más uno)",
             addition_function->GetArity(), 2);
  CheckEqual("suma(3, 4)", addition_function->Evaluate({3, 4}, call_counter),
             7);
  // Recuento: 2 + 4y = 18 con y = 4.
  // Evaluate (1) + g (1) + y niveles anotados (4) + h = composición y su
  // proyección y su sucesor = 3 llamadas por nivel (12) = 18.
  CheckEqual("llamadas de suma(3, 4)", call_counter.GetCalls(), 18);

  // ---------------------------------------------------------------------------
  // Prueba 2: caso límite y = 0. Solo se evalúa g: no hay ningún paso.
  // ---------------------------------------------------------------------------
  std::cout << "2) Caso límite: f(x, 0) = g(x), sin ningún paso recursivo\n";
  call_counter.Reset();
  CheckEqual("suma(9, 0)", addition_function->Evaluate({9, 0}, call_counter),
             9);
  CheckEqual("llamadas: la recursión (1) + g (1)", call_counter.GetCalls(), 2);

  // ---------------------------------------------------------------------------
  // Prueba 3: h recibe (x, nivel, f(x, nivel)) EN ESE ORDEN. Con h = P_2^3
  // (devuelve el nivel) queda f(x, y) = y − 1 para y > 0: si el bucle pasara
  // mal el nivel, este valor cambiaría. Y f(x, 0) = g(x) = x.
  // ---------------------------------------------------------------------------
  std::cout << "3) h recibe (x, nivel, f(x, nivel)): con h = P_2^3, f(x, y) = "
               "y − 1 si y > 0\n";
  const FunctionPointer level_function =
      std::make_shared<const PrimitiveRecursion>(first_of_one, second_of_three);
  call_counter.Reset();
  CheckEqual("Rec[P_1^1, P_2^3](7, 5)",
             level_function->Evaluate({7, 5}, call_counter), 4);
  // Recuento: 1 + g (1) + 5 niveles anotados + 5 h = 12.
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 12);
  CheckEqual("Rec[P_1^1, P_2^3](7, 0)",
             level_function->Evaluate({7, 0}, call_counter), 7);

  // ---------------------------------------------------------------------------
  // Prueba 4: las recursiones incoherentes se rechazan AL CONSTRUIRLAS.
  // ---------------------------------------------------------------------------
  std::cout << "4) Recursiones inválidas: se rechazan al construirlas\n";
  CheckInvalidRecursion("Rec[Z, S]: h debería tener aridad 3 y tiene 1", [&] {
    PrimitiveRecursion(zero_function, successor_function);
  });
  CheckInvalidRecursion(
      "g nula", [&] { PrimitiveRecursion(nullptr, successor_of_accumulated); });
  CheckInvalidRecursion("h nula",
                        [&] { PrimitiveRecursion(first_of_one, nullptr); });

  std::cout << (failed_checks == 0 ? "\nTodo correcto.\n"
                                   : "\nHAY COMPROBACIONES FALLIDAS.\n");
  return failed_checks == 0 ? 0 : 1;
}