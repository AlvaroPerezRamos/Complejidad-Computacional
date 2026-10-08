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
 * @file successor_function.h
 * @brief Definición de la clase SuccessorFunction: la función básica
 * sucesor, S : ℕ → ℕ, S(x) = x + 1.
 *
 * Es el único punto del programa por el que un valor puede crecer: todas las
 * demás funciones obtienen sus resultados a partir de Z, S y proyecciones.
 * Por eso basta comprobar el desbordamiento aquí para garantizar que ningún
 * valor intermedio se sale de Natural sin que nos enteremos.
 */

#ifndef SUCCESSOR_FUNCTION_H_
#define SUCCESSOR_FUNCTION_H_

#include <limits>

#include "primitive_recursive_function.h"

/** @brief Función básica sucesor. Aridad 1. */
class SuccessorFunction : public PrimitiveRecursiveFunction {
 public:
  SuccessorFunction() : PrimitiveRecursiveFunction("S", 1) {};

 protected:
  /**
   * @throw NaturalOverflowError Si x ya es el mayor Natural representable.
   */
  Natural Compute(const Arguments& arguments,
                                     CallCounter& /*call_counter*/) const override {
    const Natural value = arguments[0];
    // Si value ya es el máximo, value + 1 daría la vuelta a 0 SIN avisar.
    if (value == std::numeric_limits<Natural>::max()) {
      throw NaturalOverflowError(
          "El resultado (o un valor intermedio) no cabe en un natural de 64 "
          "bits.");
    }
    return value + 1;
  }
};

#endif  // SUCCESSOR_FUNCTION_H_