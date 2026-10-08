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
 * @file primitive_recursion.cc
 * @brief Implementación de la clase PrimitiveRecursion.
 */

#include <cstddef>
#include <string>
#include <utility>

#include "../include/errors.h"
#include "../include/primitive_recursion.h"

namespace {

/**
 * @brief Comprueba que g y h son coherentes.
 *
 * Se llama ANTES de usar los punteros (mismo motivo que en composition.cc: el
 * orden de evaluación de los argumentos de la clase base no está especificado).
 */
void ValidateRecursion(const FunctionPointer& base_case,
                       const FunctionPointer& recursive_step) {
  if (base_case == nullptr || recursive_step == nullptr) {
    throw InvalidFunctionDefinitionError(
        "Recursión inválida: g y h no pueden ser nulas.");
  }
  // h recibe (x, y, f(x, y)): los n parámetros más dos valores.
  if (recursive_step->GetArity() != base_case->GetArity() + 2) {
    throw InvalidFunctionDefinitionError(
        "Recursión inválida: " + base_case->GetName() + " tiene aridad " +
        std::to_string(base_case->GetArity()) + " y " +
        recursive_step->GetName() + " tiene aridad " +
        std::to_string(recursive_step->GetArity()) + " (debería ser " +
        std::to_string(base_case->GetArity() + 2) + ").");
  }
}

/** @brief Nombre legible de la recursión: "Rec[g,h]". */
std::string BuildRecursionName(const FunctionPointer& base_case,
                               const FunctionPointer& recursive_step) {
  ValidateRecursion(base_case, recursive_step);
  return "Rec[" + base_case->GetName() + "," + recursive_step->GetName() + "]";
}

/** @brief Aridad de f: la de g más uno (la variable de recursión). */
std::size_t DeduceRecursionArity(const FunctionPointer& base_case,
                                 const FunctionPointer& recursive_step) {
  ValidateRecursion(base_case, recursive_step);
  return base_case->GetArity() + 1;
}

}  // namespace

PrimitiveRecursion::PrimitiveRecursion(FunctionPointer base_case,
                                       FunctionPointer recursive_step)
    : PrimitiveRecursiveFunction(
          BuildRecursionName(base_case, recursive_step),
          DeduceRecursionArity(base_case, recursive_step)),
      base_case_(std::move(base_case)),
      recursive_step_(std::move(recursive_step)) {}

Natural PrimitiveRecursion::Compute(const Arguments& arguments,
                                    CallCounter& call_counter) const {
  // Los primeros n argumentos son los parámetros x; el último, la variable y.
  const std::size_t parameters_count = base_case_->GetArity();
  const Arguments parameters(
      arguments.begin(),
      arguments.begin() + static_cast<std::ptrdiff_t>(parameters_count));
  const Natural recursion_variable = arguments[parameters_count];

  // f(x, 0) = g(x)
  Natural accumulated_value = base_case_->Evaluate(parameters, call_counter);

  // Argumentos de h: (x, nivel, f(x, nivel)). Se reutiliza el mismo vector en
  // todas las iteraciones para no reservar memoria en cada paso.
  const std::size_t level_position = parameters_count;
  const std::size_t accumulated_position = parameters_count + 1;
  Arguments step_arguments = parameters;
  step_arguments.push_back(0);
  step_arguments.push_back(0);

  // f(x, nivel + 1) = h(x, nivel, f(x, nivel)), con nivel = 0, 1, ..., y − 1.
  for (Natural level = 0; level < recursion_variable; ++level) {
    call_counter.RegisterCall();  // La invocación de f(x, nivel) de la
                                  // definición literal.
    step_arguments[level_position] = level;
    step_arguments[accumulated_position] = accumulated_value;
    accumulated_value = recursive_step_->Evaluate(step_arguments, call_counter);
  }
  return accumulated_value;
}