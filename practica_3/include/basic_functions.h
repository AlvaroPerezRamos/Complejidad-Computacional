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
 * @file basic_functions.h
 * @brief Las tres funciones básicas de las funciones primitivas recursivas:
 * cero (Z), sucesor (S) y proyecciones (Pᵢⁿ). Son de una o dos líneas cada
 * una, así que van juntas y definidas dentro de la clase (inline), sin .cc.
 *
 * S es el único punto del programa por el que un valor puede crecer: todas
 * las demás funciones obtienen sus resultados a partir de Z, S y
 * proyecciones. Por eso basta comprobar el desbordamiento en S para
 * garantizar que ningún valor intermedio se sale de Natural sin que nos
 * enteremos.
 */

#ifndef BASIC_FUNCTIONS_H_
#define BASIC_FUNCTIONS_H_

#include <cstddef>
#include <limits>
#include <string>

#include "errors.h"
#include "primitive_recursive_function.h"

/** @brief Función básica cero: Z : ℕ → ℕ, Z(x) = 0. Aridad 1, como en los
 * apuntes. */
class ZeroFunction : public PrimitiveRecursiveFunction {
 public:
  ZeroFunction() : PrimitiveRecursiveFunction("Z", 1) {}

 protected:
  Natural Compute(const Arguments& /*arguments*/,
                  CallCounter& /*call_counter*/) const override {
    return 0;
  }
};

/** @brief Función básica sucesor: S : ℕ → ℕ, S(x) = x + 1. Aridad 1. */
class SuccessorFunction : public PrimitiveRecursiveFunction {
 public:
  SuccessorFunction() : PrimitiveRecursiveFunction("S", 1) {}

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

/** @brief Función básica proyección: Pᵢⁿ : ℕⁿ → ℕ, Pᵢⁿ(x1, ..., xn) = xi. */
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

#endif  // BASIC_FUNCTIONS_H_