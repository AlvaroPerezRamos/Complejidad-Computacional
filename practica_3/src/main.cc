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
 * @brief main TEMPORAL del paso 1 (cimientos). Comprueba que
 * PrimitiveRecursiveFunction cumple lo que promete: validar la aridad y
 * contar las llamadas. Se sustituye en el paso 2.
 *
 * PROBLEMA: PrimitiveRecursiveFunction es ABSTRACTA (Compute() no tiene
 * cuerpo), así que no se puede crear ningún objeto de esa clase, y todavía no
 * existe ninguna función concreta (Z, S y P llegan en el paso 2).
 * SOLUCIÓN: definir aquí, solo para probar, dos funciones "de juguete" que
 * heredan de ella.
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "../include/call_counter.h"
#include "../include/errors.h"
#include "../include/primitive_recursive_function.h"

namespace {

/**
 * @brief Función de juguete nº 1: ignora sus argumentos y devuelve siempre el
 * mismo valor. Sirve para probar la parte más simple de Evaluate().
 */
class ConstantFunction : public PrimitiveRecursiveFunction {
 public:
  ConstantFunction(Natural constant_value, std::size_t arity)
      : PrimitiveRecursiveFunction(
            "Constante(" + std::to_string(constant_value) + ")", arity),
        constant_value_(constant_value) {}

 protected:
  Natural Compute(const Arguments& /*arguments*/,
                  CallCounter& /*call_counter*/) const override {
    return constant_value_;
  }

 private:
  Natural constant_value_;
};

/**
 * @brief Función de juguete nº 2 (aridad 1): para calcularse evalúa DOS veces
 * otra función. Sirve para ver que las llamadas anidadas también se cuentan
 * sin que esta clase tenga que hacer nada para contarlas.
 *
 * Guarda la función interior como FunctionPointer (un shared_ptr a const):
 * varias funciones pueden compartir una misma sub-función sin copiarla y sin
 * preocuparse de quién debe liberarla. Se usará en todo el proyecto.
 */
class EvaluatesInnerTwiceFunction : public PrimitiveRecursiveFunction {
 public:
  explicit EvaluatesInnerTwiceFunction(FunctionPointer inner_function)
      : PrimitiveRecursiveFunction(
            "DosVeces(" + inner_function->GetName() + ")", 1),
        inner_function_(std::move(inner_function)) {}

 protected:
  Natural Compute(const Arguments& arguments,
                  CallCounter& call_counter) const override {
    inner_function_->Evaluate(arguments,
                              call_counter);  // 1.ª evaluación (se descarta)
    return inner_function_->Evaluate(
        arguments, call_counter);  // 2.ª evaluación (es el resultado)
  }

 private:
  FunctionPointer inner_function_;
};

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
  // Un único contador para toda la prueba; se pone a cero entre pruebas con
  // Reset().
  CallCounter call_counter;

  // ---------------------------------------------------------------------------
  // Prueba 1: una evaluación simple devuelve su valor y cuenta UNA llamada.
  // ---------------------------------------------------------------------------
  std::cout
      << "1) Una evaluación simple devuelve su valor y cuenta UNA llamada\n";
  // Función de aridad 2 que siempre devuelve 7: la evaluamos con 2 argumentos.
  const auto constant_seven = std::make_shared<const ConstantFunction>(7, 2);
  CheckEqual("Constante(7)(1, 2)",
             constant_seven->Evaluate({1, 2}, call_counter), 7);
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 1);

  // ---------------------------------------------------------------------------
  // Prueba 2: el contador ACUMULA entre evaluaciones hasta que se hace Reset().
  // ---------------------------------------------------------------------------
  std::cout
      << "2) El contador acumula entre evaluaciones; Reset() lo pone a cero\n";
  constant_seven->Evaluate(
      {3, 4}, call_counter);  // Segunda evaluación con el MISMO contador.
  CheckEqual("llamadas tras una segunda evaluación", call_counter.GetCalls(),
             2);
  call_counter.Reset();
  CheckEqual("llamadas tras Reset()", call_counter.GetCalls(), 0);

  // ---------------------------------------------------------------------------
  // Prueba 3: las llamadas ANIDADAS se cuentan solas. Es la razón de que sea
  // Evaluate() (y no cada subclase) quien anota la llamada.
  // ---------------------------------------------------------------------------
  std::cout << "3) Las llamadas anidadas se cuentan: 1 (exterior) + 2 "
               "(interiores) = 3\n";
  const FunctionPointer constant_five =
      std::make_shared<const ConstantFunction>(5, 1);
  const EvaluatesInnerTwiceFunction evaluates_twice(constant_five);
  CheckEqual("DosVeces(Constante(5))(0)",
             evaluates_twice.Evaluate({0}, call_counter), 5);
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 3);

  // ---------------------------------------------------------------------------
  // Prueba 4: aridad incorrecta. Se lanza ArityMismatchError y NO se cuenta la
  // llamada, porque la comprobación de aridad va ANTES de RegisterCall().
  // ---------------------------------------------------------------------------
  std::cout << "4) Aridad incorrecta: lanza ArityMismatchError y NO cuenta la "
               "llamada\n";
  call_counter.Reset();
  bool arity_error_thrown = false;
  try {
    constant_seven->Evaluate(
        {1}, call_counter);  // La función espera 2 argumentos y recibe 1.
  } catch (const ArityMismatchError& error) {
    arity_error_thrown = true;
    std::cout << "  mensaje: " << error.what() << "\n";
  }
  CheckEqual("se lanzó ArityMismatchError (1 = sí)", arity_error_thrown ? 1 : 0,
             1);
  CheckEqual("llamadas contadas", call_counter.GetCalls(), 0);

  // ---------------------------------------------------------------------------
  // Prueba 5: captadores.
  // ---------------------------------------------------------------------------
  std::cout << "5) Captadores\n";
  CheckEqual("GetArity()", constant_seven->GetArity(), 2);
  std::cout << "  GetName(): " << constant_seven->GetName() << "\n";

  std::cout << (failed_checks == 0 ? "\nTodo correcto.\n"
                                   : "\nHAY COMPROBACIONES FALLIDAS.\n");
  return failed_checks == 0 ? 0 : 1;
}
