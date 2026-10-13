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
 * @file primitive_recursive_function.cc
 * @brief Implementación de la clase PrimitiveRecursiveFunction.
 */

#include "../include/primitive_recursive_function.h"

#include <utility>

#include "../include/errors.h"

PrimitiveRecursiveFunction::PrimitiveRecursiveFunction(std::string name,
                                                       std::size_t arity,
                                                       bool counts_call)
    : name_(std::move(name)), arity_(arity), counts_call_(counts_call) {}

Natural PrimitiveRecursiveFunction::Evaluate(const Arguments& arguments,
                                             CallCounter& call_counter) const {
  if (arguments.size() != arity_) {
    throw ArityMismatchError("La función " + name_ + " espera " +
                             std::to_string(arity_) +
                             " argumento(s) y ha recibido " +
                             std::to_string(arguments.size()) + ".");
  }
  if (counts_call_) {
    call_counter.RegisterCall();
  }
  return Compute(arguments, call_counter);
}