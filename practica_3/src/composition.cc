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
 * @file composition.cc
 * @brief Implementación de la clase Composition.
 */

#include "../include/composition.h"

#include <string>
#include <utility>

#include "../include/errors.h"

namespace {

/**
 * @brief Comprueba que los componentes de una composición son coherentes.
 *
 * Se llama ANTES de usar los punteros (para construir el nombre o deducir la
 * aridad), porque el orden en que se evalúan los argumentos del constructor
 * de la clase base no está especificado: sin esto, un puntero nulo se
 * dereferenciaría antes de poder rechazarlo.
 */
void ValidateComposition(const FunctionPointer& outer_function,
                         const std::vector<FunctionPointer>& inner_functions) {
  if (outer_function == nullptr) {
    throw InvalidFunctionDefinitionError(
        "Composición inválida: la función exterior es nula.");
  }
  if (inner_functions.empty()) {
    throw InvalidFunctionDefinitionError(
        "Composición inválida: se necesita al menos una función interior.");
  }
  for (const FunctionPointer& inner_function : inner_functions) {
    if (inner_function == nullptr) {
      throw InvalidFunctionDefinitionError(
          "Composición inválida: una función interior es nula.");
    }
  }
  // f : ℕᵐ → ℕ necesita exactamente m valores, uno por cada gi.
  if (outer_function->GetArity() != inner_functions.size()) {
    throw InvalidFunctionDefinitionError(
        "Composición inválida: " + outer_function->GetName() +
        " tiene aridad " + std::to_string(outer_function->GetArity()) +
        " pero se le dan " + std::to_string(inner_functions.size()) +
        " función(es) interior(es).");
  }
  // Todas las gi se evalúan sobre los MISMOS argumentos: deben tener la misma
  // aridad n.
  const std::size_t common_arity = inner_functions.front()->GetArity();
  for (const FunctionPointer& inner_function : inner_functions) {
    if (inner_function->GetArity() != common_arity) {
      throw InvalidFunctionDefinitionError(
          "Composición inválida: las funciones interiores deben tener la misma "
          "aridad (" +
          inner_functions.front()->GetName() + " tiene " +
          std::to_string(common_arity) + ", " + inner_function->GetName() +
          " tiene " + std::to_string(inner_function->GetArity()) + ").");
    }
  }
}

/** @brief Nombre legible de la composición: "f∘(g1,...,gm)". */
std::string BuildCompositionName(
    const FunctionPointer& outer_function,
    const std::vector<FunctionPointer>& inner_functions) {
  ValidateComposition(outer_function, inner_functions);
  std::string name = outer_function->GetName() + "∘(";
  for (std::size_t i = 0; i < inner_functions.size(); ++i) {
    if (i > 0) name += ",";
    name += inner_functions[i]->GetName();
  }
  return name + ")";
}

/** @brief Aridad de la composición: la común de las funciones interiores. */
std::size_t DeduceCompositionArity(
    const FunctionPointer& outer_function,
    const std::vector<FunctionPointer>& inner_functions) {
  ValidateComposition(outer_function, inner_functions);
  return inner_functions.front()->GetArity();
}

}  // namespace

Composition::Composition(FunctionPointer outer_function,
                         std::vector<FunctionPointer> inner_functions)
    : PrimitiveRecursiveFunction(
          BuildCompositionName(outer_function, inner_functions),
          DeduceCompositionArity(outer_function, inner_functions)),
      outer_function_(std::move(outer_function)),
      inner_functions_(std::move(inner_functions)) {}

Natural Composition::Compute(const Arguments& arguments,
                             CallCounter& call_counter) const {
  // Tupla (g1(x), ..., gm(x)): cada gi se evalúa sobre los argumentos
  // ORIGINALES.
  Arguments inner_results;
  inner_results.reserve(inner_functions_.size());
  for (const FunctionPointer& inner_function : inner_functions_) {
    inner_results.push_back(inner_function->Evaluate(arguments, call_counter));
  }
  // f recibe la tupla de resultados, no los argumentos originales.
  return outer_function_->Evaluate(inner_results, call_counter);
}