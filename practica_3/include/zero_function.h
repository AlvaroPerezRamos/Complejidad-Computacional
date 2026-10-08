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
 * @file zero_function.h
 * @brief Definición de la clase ZeroFunction: la función básica cero,
 * Z : ℕ → ℕ, Z(x) = 0.
 */

#ifndef ZERO_FUNCTION_H_
#define ZERO_FUNCTION_H_

#include "primitive_recursive_function.h"

/** @brief Función básica cero. Aridad 1, como en los apuntes de clase. */
class ZeroFunction : public PrimitiveRecursiveFunction {
 public:
  ZeroFunction(): PrimitiveRecursiveFunction("Z", 1) {}

 protected:
  Natural Compute(const Arguments& /*arguments*/,
                  CallCounter& /*call_counter*/) const override {
    return 0;
  }
};

#endif  // ZERO_FUNCTION_H_