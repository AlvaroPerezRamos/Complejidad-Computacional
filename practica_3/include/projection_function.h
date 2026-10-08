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
 * @file projection_function.h
 * @brief Definición de la clase ProjectionFunction: las funciones básicas
 * proyección, Pᵢⁿ : ℕⁿ → ℕ, Pᵢⁿ(x1, ..., xn) = xi.
 */

#ifndef PROJECTION_FUNCTION_H_
#define PROJECTION_FUNCTION_H_

#include <cstddef>

#include "primitive_recursive_function.h"


/** @brief Proyección i-ésima de n argumentos. */
class ProjectionFunction : public PrimitiveRecursiveFunction {
 public:
  /**
   * @param position Posición i del argumento que se devuelve, contada desde 1
   * (igual que en los apuntes: P₁³(x, y, z) = x).
   * @param arity Número n de argumentos.
   * @throw InvalidFunctionDefinitionError Si no se cumple 1 ≤ i ≤ n.
   */
  ProjectionFunction(std::size_t position, std::size_t arity)
      : PrimitiveRecursiveFunction(
            "P_" + std::to_string(position) + "^" + std::to_string(arity),
            arity),
        position_(position) {
    if (position_ < 1 || position_ > arity) {
      throw InvalidFunctionDefinitionError("Proyección inválida " + GetName() +
                                           ": debe cumplirse 1 <= i <= n.");
    }
  }

 protected:
  Natural Compute(const Arguments& arguments,
                  CallCounter& /*call_counter*/) const override {
    // position_ cuenta desde 1 y los vectores desde 0. Este "- 1" es un ÍNDICE
    // de std::vector, no una resta sobre un valor de ℕ.
    return arguments[position_ - 1];
  }

 private:
  std::size_t position_;  // Posición i (desde 1) del argumento a devolver.
};

#endif  // PROJECTION_FUNCTION_H_