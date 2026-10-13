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
 * @file named_function.cc
 * @brief Implementación de la clase NamedFunction.
 */

#include "../include/named_function.h"

#include <cstddef>
#include <string>
#include <utility>

#include "../include/errors.h"

namespace {

/**
 * @brief Devuelve la aridad de la definición, rechazándola antes si es nula.
 *
 * Se llama ANTES de usar el puntero (mismo motivo que en composition.cc: el
 * orden de evaluación de los argumentos de la clase base no está especificado).
 */
std::size_t DeduceNamedArity(const FunctionPointer& definition) {
  if (definition == nullptr) {
    throw InvalidFunctionDefinitionError(
        "Función con nombre inválida: la definición es nula.");
  }
  return definition->GetArity();
}

}  // namespace

NamedFunction::NamedFunction(std::string name, FunctionPointer definition)
    : PrimitiveRecursiveFunction(std::move(name), DeduceNamedArity(definition)),
      definition_(std::move(definition)) {}

Natural NamedFunction::Compute(const Arguments& arguments,
                               CallCounter& call_counter) const {
  return definition_->Evaluate(arguments, call_counter);
}