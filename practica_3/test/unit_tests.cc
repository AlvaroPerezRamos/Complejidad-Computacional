/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 3: Funciones primitivas recursivas de números naturales
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 09/10/2026
 * @file unit_tests.cc
 * @brief Pruebas unitarias de la Práctica 3. Cubren lo que la línea de
 * comandos no puede alcanzar: funciones básicas, plantilla de evaluación,
 * definiciones inválidas (combinación, composición, recursión), desbordamiento
 * y aridades, además de los valores y recuentos de llamadas de FunctionLibrary,
 * contrastados con un oráculo independiente (recursión literal con contador).
 * Salida: 0 si todo pasa, 1 si alguna comprobación falla.
 */

#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "../include/basic_functions.h"
#include "../include/call_counter.h"
#include "../include/combination.h"
#include "../include/composition.h"
#include "../include/errors.h"
#include "../include/function_library.h"
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
 * @brief Comprueba que construir algo inválido lanza
 * InvalidFunctionDefinitionError.
 * @param description Qué tiene de inválido.
 * @param build Código que intenta construirlo.
 */
void CheckInvalidDefinition(const std::string& description,
                            const std::function<void()>& build) {
  try {
    build();
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
  const FunctionPointer first_of_two =
      std::make_shared<const ProjectionFunction>(1, 2);
  const FunctionPointer second_of_two =
      std::make_shared<const ProjectionFunction>(2, 2);
  const FunctionPointer second_of_three =
      std::make_shared<const ProjectionFunction>(2, 3);
  const FunctionPointer third_of_three =
      std::make_shared<const ProjectionFunction>(3, 3);
  CallCounter call_counter;

  // ===========================================================================
  // FUNCIONES BÁSICAS Y PLANTILLA DE EVALUACIÓN
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Prueba 1: Z(x) = 0 para cualquier x.
  // ---------------------------------------------------------------------------
  std::cout << "1) Z(x) = 0, para cualquier x\n";
  CheckEqual("Z(7)", zero_function->Evaluate({7}, call_counter), 0);
  CheckEqual("Z(0)", zero_function->Evaluate({0}, call_counter), 0);

  // ---------------------------------------------------------------------------
  // Prueba 2: S(x) = x + 1.
  // ---------------------------------------------------------------------------
  std::cout << "2) S(x) = x + 1\n";
  CheckEqual("S(0)", successor_function->Evaluate({0}, call_counter), 1);
  CheckEqual("S(4)", successor_function->Evaluate({4}, call_counter), 5);

  // ---------------------------------------------------------------------------
  // Prueba 3: Pᵢⁿ devuelve el argumento i-ésimo (i cuenta desde 1).
  // ---------------------------------------------------------------------------
  std::cout << "3) Pᵢⁿ devuelve el argumento i-ésimo (i cuenta desde 1)\n";
  const ProjectionFunction first_of_three(1, 3);
  CheckEqual("P_1^3(10, 20, 30)",
             first_of_three.Evaluate({10, 20, 30}, call_counter), 10);
  CheckEqual("P_2^3(10, 20, 30)",
             second_of_three->Evaluate({10, 20, 30}, call_counter), 20);
  CheckEqual("P_3^3(10, 20, 30)",
             third_of_three->Evaluate({10, 20, 30}, call_counter), 30);
  std::cout << "  nombre: " << second_of_three->GetName() << ", aridad "
            << second_of_three->GetArity() << "\n";

  // ---------------------------------------------------------------------------
  // Prueba 4: cada evaluación cuenta exactamente UNA llamada (2 + 2 + 3 = 7).
  // ---------------------------------------------------------------------------
  std::cout
      << "4) Cada evaluación cuenta exactamente una llamada (2 + 2 + 3 = 7)\n";
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 7);
  call_counter.Reset();
  CheckEqual("tras Reset()", call_counter.GetCalls(), 0);

  // ---------------------------------------------------------------------------
  // Prueba 5: proyección mal construida: debe cumplirse 1 <= i <= n.
  // ---------------------------------------------------------------------------
  std::cout << "5) Proyección mal construida: debe cumplirse 1 <= i <= n\n";
  CheckInvalidDefinition("P_0^3 (i demasiado pequeño)",
                         [&] { ProjectionFunction(0, 3); });
  CheckInvalidDefinition("P_4^3 (i mayor que n)",
                         [&] { ProjectionFunction(4, 3); });

  // ---------------------------------------------------------------------------
  // Prueba 6: desbordamiento. S es el único punto donde un valor puede crecer.
  // ---------------------------------------------------------------------------
  std::cout
      << "6) Desbordamiento: S(máximo natural) lanza NaturalOverflowError\n";
  const Natural largest_natural = std::numeric_limits<Natural>::max();
  CheckEqual("S(máximo - 1)",
             successor_function->Evaluate({largest_natural - 1}, call_counter),
             largest_natural);
  try {
    successor_function->Evaluate({largest_natural}, call_counter);
    std::cout << "  [FALLO] S(máximo) no lanzó excepción\n";
    ++failed_checks;
  } catch (const NaturalOverflowError& error) {
    std::cout << "  [OK]    " << error.what() << "\n";
  }

  // ---------------------------------------------------------------------------
  // Prueba 7: aridad incorrecta al evaluar. La plantilla Evaluate la comprueba
  // ANTES de anotar la llamada: una evaluación rechazada no cuenta.
  // ---------------------------------------------------------------------------
  std::cout
      << "7) Aridad incorrecta: ArityMismatchError y la llamada NO se cuenta\n";
  call_counter.Reset();
  try {
    zero_function->Evaluate({1, 2}, call_counter);
    std::cout << "  [FALLO] Z(1, 2) no lanzó excepción\n";
    ++failed_checks;
  } catch (const ArityMismatchError& error) {
    std::cout << "  [OK]    " << error.what() << "\n";
  }
  CheckEqual("llamadas contadas tras la evaluación rechazada",
             call_counter.GetCalls(), 0);

  // ===========================================================================
  // COMBINACIÓN
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Prueba 8: la combinación devuelve la TUPLA, en el orden de las funciones.
  // ---------------------------------------------------------------------------
  std::cout
      << "8) Combinación (P_2^2, P_1^2) evaluada en (5, 8): la tupla (8, 5)\n";
  const Combination swap_combination(
      std::vector<FunctionPointer>{second_of_two, first_of_two});
  const Arguments swapped_tuple =
      swap_combination.Evaluate({5, 8}, call_counter);
  CheckEqual("tamaño de la tupla", swapped_tuple.size(), 2);
  CheckEqual("componente 1 (P_2^2)", swapped_tuple[0], 8);
  CheckEqual("componente 2 (P_1^2)", swapped_tuple[1], 5);
  // La combinación no anota llamada propia: solo las dos proyecciones.
  CheckEqual("llamadas contadas (solo las de las gi)", call_counter.GetCalls(),
             2);
  CheckEqual("aridad n (de los argumentos)", swap_combination.GetArity(), 2);
  CheckEqual("tamaño m (de la tupla)", swap_combination.GetSize(), 2);
  std::cout << "  nombre: " << swap_combination.GetName() << "\n";

  // ---------------------------------------------------------------------------
  // Prueba 9: combinaciones incoherentes, rechazadas al construirlas.
  // ---------------------------------------------------------------------------
  std::cout << "9) Combinaciones inválidas: se rechazan al construirlas\n";
  CheckInvalidDefinition("sin funciones",
                         [&] { Combination(std::vector<FunctionPointer>{}); });
  CheckInvalidDefinition("una función nula", [&] {
    Combination(std::vector<FunctionPointer>{zero_function, nullptr});
  });
  CheckInvalidDefinition("P_1^3 y P_1^2: aridades distintas", [&] {
    const FunctionPointer first_of_three =
        std::make_shared<const ProjectionFunction>(1, 3);
    Combination(std::vector<FunctionPointer>{first_of_three, first_of_two});
  });

  // ===========================================================================
  // COMPOSICIÓN
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Prueba 10: uno = S ∘ Z, es decir uno(x) = S(Z(x)) = 1.
  // ---------------------------------------------------------------------------
  std::cout << "10) uno = S ∘ Z, es decir uno(x) = S(Z(x)) = 1\n";
  const FunctionPointer one_function = std::make_shared<const Composition>(
      successor_function,
      Combination(std::vector<FunctionPointer>{zero_function}));
  call_counter.Reset();
  CheckEqual("uno(9)", one_function->Evaluate({9}, call_counter), 1);
  // Llamadas: la composición (1) + Z (1) + S (1).
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 3);
  std::cout << "  nombre: " << one_function->GetName() << "\n";

  // ---------------------------------------------------------------------------
  // Prueba 11: una composición puede contener otra composición.
  // ---------------------------------------------------------------------------
  std::cout
      << "11) Composición de composiciones: dos = S ∘ uno = S(S(Z(x))) = 2\n";
  const FunctionPointer two_function = std::make_shared<const Composition>(
      successor_function,
      Combination(std::vector<FunctionPointer>{one_function}));
  call_counter.Reset();
  CheckEqual("dos(9)", two_function->Evaluate({9}, call_counter), 2);
  // Llamadas: composición exterior (1) + uno (3) + S (1).
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 5);

  // ---------------------------------------------------------------------------
  // Prueba 12: varias funciones en la combinación. f recibe la TUPLA
  // (g1(x), g2(x)), no los argumentos originales.
  // ---------------------------------------------------------------------------
  std::cout << "12) Varias funciones: P_1^2 ∘ (P_2^2, P_1^2) (x, y)\n";
  std::cout << "   La exterior recibe la tupla (y, x) y devuelve su primer "
               "valor: y\n";
  const Composition swapped_first(first_of_two, swap_combination);
  call_counter.Reset();
  CheckEqual("P_1^2 ∘ (P_2^2, P_1^2) (5, 8)",
             swapped_first.Evaluate({5, 8}, call_counter), 8);
  // Llamadas: composición (1) + dos de la combinación (2) + exterior (1).
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 4);
  CheckEqual("aridad de la composición (la de la combinación)",
             swapped_first.GetArity(), 2);

  // ---------------------------------------------------------------------------
  // Prueba 13: composiciones incoherentes, rechazadas al construirlas.
  // ---------------------------------------------------------------------------
  std::cout << "13) Composiciones inválidas: se rechazan al construirlas\n";
  CheckInvalidDefinition(
      "S ∘ (Z, Z): S tiene aridad 1 y la combinación da 2 valores", [&] {
        Composition(successor_function,
                    Combination(std::vector<FunctionPointer>{zero_function,
                                                             zero_function}));
      });
  CheckInvalidDefinition("función exterior nula", [&] {
    Composition(nullptr,
                Combination(std::vector<FunctionPointer>{zero_function}));
  });

  // ===========================================================================
  // RECURSIÓN PRIMITIVA
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Prueba 14: la suma, construida a mano con g = P_1^1 y h = S ∘ P_3^3.
  // ---------------------------------------------------------------------------
  std::cout << "14) suma = Rec[P_1^1, S ∘ P_3^3]\n";
  const FunctionPointer successor_of_accumulated =
      std::make_shared<const Composition>(
          successor_function,
          Combination(std::vector<FunctionPointer>{third_of_three}));
  const FunctionPointer addition_function =
      std::make_shared<const PrimitiveRecursion>(first_of_one,
                                                 successor_of_accumulated);
  std::cout << "  nombre: " << addition_function->GetName() << ", aridad "
            << addition_function->GetArity() << "\n";
  CheckEqual("aridad de la suma (la de g más uno)",
             addition_function->GetArity(), 2);
  call_counter.Reset();
  CheckEqual("suma(3, 4)", addition_function->Evaluate({3, 4}, call_counter),
             7);
  // Recuento: 2 + 4y = 18 con y = 4.
  // Evaluate (1) + g (1) + y niveles anotados (4) + h = composición y su
  // proyección y su sucesor = 3 llamadas por nivel (12) = 18.
  CheckEqual("llamadas de suma(3, 4)", call_counter.GetCalls(), 18);

  // ---------------------------------------------------------------------------
  // Prueba 15: caso límite y = 0. Solo se evalúa g: no hay ningún paso.
  // ---------------------------------------------------------------------------
  std::cout << "15) Caso límite: f(x, 0) = g(x), sin ningún paso recursivo\n";
  call_counter.Reset();
  CheckEqual("suma(9, 0)", addition_function->Evaluate({9, 0}, call_counter),
             9);
  CheckEqual("llamadas: la recursión (1) + g (1)", call_counter.GetCalls(), 2);

  // ---------------------------------------------------------------------------
  // Prueba 16: h recibe (x, nivel, f(x, nivel)) EN ESE ORDEN. Con h = P_2^3
  // (devuelve el nivel) queda f(x, y) = y − 1 para y > 0: si el bucle pasara
  // mal el nivel, este valor cambiaría. Y f(x, 0) = g(x) = x.
  // ---------------------------------------------------------------------------
  std::cout << "16) h recibe (x, nivel, f(x, nivel)): con h = P_2^3, f(x, y) = "
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
  // Prueba 17: las recursiones incoherentes se rechazan AL CONSTRUIRLAS.
  // ---------------------------------------------------------------------------
  std::cout << "17) Recursiones inválidas: se rechazan al construirlas\n";
  CheckInvalidDefinition("Rec[Z, S]: h debería tener aridad 3 y tiene 1", [&] {
    PrimitiveRecursion(zero_function, successor_function);
  });
  CheckInvalidDefinition(
      "g nula", [&] { PrimitiveRecursion(nullptr, successor_of_accumulated); });
  CheckInvalidDefinition("h nula",
                         [&] { PrimitiveRecursion(first_of_one, nullptr); });

  // ===========================================================================
  // LIBRERÍA DE FUNCIONES
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Prueba 18: uno y suma construidas por la librería.
  // ---------------------------------------------------------------------------
  std::cout << "18) FunctionLibrary: uno y suma (suma: 2 + 4y llamadas)\n";
  const FunctionPointer library_one = FunctionLibrary::One();
  const FunctionPointer library_addition = FunctionLibrary::Addition();
  call_counter.Reset();
  CheckEqual("uno(9)", library_one->Evaluate({9}, call_counter), 1);
  CheckEqual("llamadas de uno(9)", call_counter.GetCalls(), 3);
  call_counter.Reset();
  CheckEqual("suma(3, 4)", library_addition->Evaluate({3, 4}, call_counter), 7);
  CheckEqual("llamadas de suma(3, 4) = 2 + 4·4", call_counter.GetCalls(), 18);
  call_counter.Reset();
  CheckEqual("suma(7, 0)", library_addition->Evaluate({7, 0}, call_counter), 7);
  CheckEqual("llamadas de suma(7, 0) = 2", call_counter.GetCalls(), 2);

  // ---------------------------------------------------------------------------
  // Prueba 19: producto. Llamadas: 2 + 6y + 4·x·y.
  // ---------------------------------------------------------------------------
  std::cout << "19) FunctionLibrary: producto (2 + 6y + 4xy llamadas)\n";
  const FunctionPointer library_multiplication =
      FunctionLibrary::Multiplication();
  call_counter.Reset();
  CheckEqual("producto(3, 4)",
             library_multiplication->Evaluate({3, 4}, call_counter), 12);
  CheckEqual("llamadas de producto(3, 4) = 2 + 24 + 48",
             call_counter.GetCalls(), 74);
  call_counter.Reset();
  CheckEqual("producto(5, 0)",
             library_multiplication->Evaluate({5, 0}, call_counter), 0);
  CheckEqual("llamadas de producto(5, 0) = 2", call_counter.GetCalls(), 2);
  call_counter.Reset();
  CheckEqual("producto(0, 5)",
             library_multiplication->Evaluate({0, 5}, call_counter), 0);
  CheckEqual("llamadas de producto(0, 5) = 2 + 30", call_counter.GetCalls(),
             32);

  // ---------------------------------------------------------------------------
  // Prueba 20: potencia. Es el objetivo de la práctica.
  // ---------------------------------------------------------------------------
  std::cout << "20) FunctionLibrary: potencia (con la convención 0^0 = 1)\n";
  const FunctionPointer library_power = FunctionLibrary::Power();
  std::cout << "  nombre: " << library_power->GetName() << "\n";
  call_counter.Reset();
  CheckEqual("potencia(2, 3)", library_power->Evaluate({2, 3}, call_counter),
             8);
  CheckEqual("llamadas de potencia(2, 3)", call_counter.GetCalls(), 114);
  call_counter.Reset();
  CheckEqual("potencia(3, 2)", library_power->Evaluate({3, 2}, call_counter),
             9);
  CheckEqual("llamadas de potencia(3, 2)", call_counter.GetCalls(), 100);
  call_counter.Reset();
  CheckEqual("potencia(5, 0) = 1",
             library_power->Evaluate({5, 0}, call_counter), 1);
  CheckEqual("llamadas de potencia(5, 0) = 1 + uno (3)",
             call_counter.GetCalls(), 4);
  call_counter.Reset();
  CheckEqual("potencia(0, 0) = 1 (convención)",
             library_power->Evaluate({0, 0}, call_counter), 1);
  call_counter.Reset();
  CheckEqual("potencia(0, 3) = 0",
             library_power->Evaluate({0, 3}, call_counter), 0);
  CheckEqual("llamadas de potencia(0, 3)", call_counter.GetCalls(), 22);
  call_counter.Reset();
  CheckEqual("potencia(2, 20)", library_power->Evaluate({2, 20}, call_counter),
             1048576);
  CheckEqual("llamadas de potencia(2, 20)", call_counter.GetCalls(), 8388964);

  // ---------------------------------------------------------------------------
  // Prueba 21: una función de la librería exige su aridad al evaluarla.
  // ---------------------------------------------------------------------------
  std::cout
      << "21) Aridad al evaluar: potencia(2) debe lanzar ArityMismatchError\n";
  try {
    library_power->Evaluate({2}, call_counter);
    std::cout << "  [FALLO] potencia(2) no lanzó excepción\n";
    ++failed_checks;
  } catch (const ArityMismatchError& error) {
    std::cout << "  [OK]    " << error.what() << "\n";
  }

  std::cout << (failed_checks == 0 ? "\nTodo correcto.\n"
                                   : "\nHAY COMPROBACIONES FALLIDAS.\n");
  return failed_checks == 0 ? 0 : 1;
}