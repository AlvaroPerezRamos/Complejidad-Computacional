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
 * @file composition.h
 * @brief Definición de la clase Composition: la operación de composición,
 * h = f ∘ (g1, ..., gm), con f : ℕᵐ → ℕ y gi : ℕⁿ → ℕ; resulta h : ℕⁿ → ℕ,
 * h(x) = f(g1(x), ..., gm(x)).
 *
 * Incluye la "combinación" de los apuntes: la tupla (g1(x), ..., gm(x)) ∈ ℕᵐ
 * se construye evaluando las gi sobre los mismos argumentos y se le entrega a
 * f.
 *
 * Es una función más (hereda de PrimitiveRecursiveFunction) que CONTIENE
 * otras funciones: así se pueden componer composiciones y formar árboles de
 * funciones tan profundos como haga falta.
 */

#ifndef COMPOSITION_H_
#define COMPOSITION_H_

#include <vector>

#include "primitive_recursive_function.h"

/** @brief Composición h = f ∘ (g1, ..., gm). */
class Composition : public PrimitiveRecursiveFunction {
 public:
  /**
   * @param outer_function Función f, de aridad m.
   * @param inner_functions Funciones g1..gm: deben ser exactamente m, no
   * nulas, y todas con la misma aridad n (que será la de la composición).
   * @throw InvalidFunctionDefinitionError Si algún requisito anterior falla.
   */
  Composition(FunctionPointer outer_function,
              std::vector<FunctionPointer> inner_functions);

 protected:
  /** @brief Evalúa las gi sobre arguments y entrega la tupla resultante a f. */
  Natural Compute(const Arguments& arguments,
                  CallCounter& call_counter) const override;

 private:
  FunctionPointer outer_function_;                // f
  std::vector<FunctionPointer> inner_functions_;  // g1, ..., gm
};

#endif  // COMPOSITION_H_