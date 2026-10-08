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
 * @brief Comprueba que f y la combinación encajan.
 *
 * Se llama ANTES de usar f (para construir el nombre), porque el orden en que
 * se evalúan los argumentos del constructor de la clase base no está
 * especificado: sin esto, un puntero nulo se dereferenciaría antes de poder
 * rechazarlo. La combinación ya llega validada por su propio constructor.
 */
void ValidateComposition(const FunctionPointer& outer_function,
                         const Combination& combination) {
  if (outer_function == nullptr) {
    throw InvalidFunctionDefinitionError(
        "Composición inválida: la función exterior es nula.");
  }
  // f : ℕᵐ → ℕ necesita exactamente m valores, uno por cada gi de la
  // combinación.
  if (outer_function->GetArity() != combination.GetSize()) {
    throw InvalidFunctionDefinitionError(
        "Composición inválida: " + outer_function->GetName() +
        " tiene aridad " + std::to_string(outer_function->GetArity()) +
        " pero la combinación " + combination.GetName() + " devuelve " +
        std::to_string(combination.GetSize()) + " valor(es).");
  }
}

/** @brief Nombre legible de la composición: "f∘(g1,...,gm)". */
std::string BuildCompositionName(const FunctionPointer& outer_function,
                                 const Combination& combination) {
  ValidateComposition(outer_function, combination);
  return outer_function->GetName() + "∘" + combination.GetName();
}

}  // namespace

Composition::Composition(FunctionPointer outer_function,
                         Combination combination)
    : PrimitiveRecursiveFunction(
          BuildCompositionName(outer_function, combination),
          combination.GetArity()),
      outer_function_(std::move(outer_function)),
      combination_(std::move(combination)) {}

Natural Composition::Compute(const Arguments& arguments,
                             CallCounter& call_counter) const {
  // Primero la combinación: la tupla (g1(x), ..., gm(x)) ...
  const Arguments combined_results =
      combination_.Evaluate(arguments, call_counter);
  // ... y f recibe esa tupla, no los argumentos originales.
  return outer_function_->Evaluate(combined_results, call_counter);
}