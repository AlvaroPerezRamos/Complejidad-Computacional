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
 * @file combination.cc
 * @brief Implementación de la clase Combination.
 */

#include "../include/combination.h"

#include <utility>

#include "../include/errors.h"

namespace {

/** @brief Comprueba que las gi existen y comparten aridad. */
void ValidateCombination(const std::vector<FunctionPointer>& inner_functions) {
  if (inner_functions.empty()) {
    throw InvalidFunctionDefinitionError(
        "Combinación inválida: se necesita al menos una función.");
  }
  for (const FunctionPointer& inner_function : inner_functions) {
    if (inner_function == nullptr) {
      throw InvalidFunctionDefinitionError(
          "Combinación inválida: una función es nula.");
    }
  }
  // Todas las gi se evalúan sobre los MISMOS argumentos: deben tener la misma
  // aridad n.
  const std::size_t common_arity = inner_functions.front()->GetArity();
  for (const FunctionPointer& inner_function : inner_functions) {
    if (inner_function->GetArity() != common_arity) {
      throw InvalidFunctionDefinitionError(
          "Combinación inválida: las funciones deben tener la misma aridad (" +
          inner_functions.front()->GetName() + " tiene " +
          std::to_string(common_arity) + ", " + inner_function->GetName() +
          " tiene " + std::to_string(inner_function->GetArity()) + ").");
    }
  }
}

}  // namespace

Combination::Combination(std::vector<FunctionPointer> inner_functions)
    : inner_functions_(std::move(inner_functions)) {
  ValidateCombination(inner_functions_);
}

Arguments Combination::Evaluate(const Arguments& arguments,
                                CallCounter& call_counter) const {
  // Tupla (g1(x), ..., gm(x)): cada gi se evalúa sobre los argumentos
  // ORIGINALES.
  Arguments results;
  results.reserve(inner_functions_.size());
  for (const FunctionPointer& inner_function : inner_functions_) {
    results.push_back(inner_function->Evaluate(arguments, call_counter));
  }
  return results;
}

std::size_t Combination::GetArity() const {
  return inner_functions_.front()->GetArity();
}

std::size_t Combination::GetSize() const { return inner_functions_.size(); }

std::string Combination::GetName() const {
  std::string name = "(";
  for (std::size_t i = 0; i < inner_functions_.size(); ++i) {
    if (i > 0) name += ",";
    name += inner_functions_[i]->GetName();
  }
  return name + ")";
}