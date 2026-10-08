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
 * Es "combinar y después aplicar f": la combinación (g1, ..., gm) (clase
 * Combination) produce la tupla (g1(x), ..., gm(x)) ∈ ℕᵐ y se la entrega a f.
 *
 * Es una función más (hereda de PrimitiveRecursiveFunction) que CONTIENE
 * otras funciones: así se pueden componer composiciones y formar árboles de
 * funciones tan profundos como haga falta.
 */

#ifndef COMPOSITION_H_
#define COMPOSITION_H_

#include "combination.h"
#include "primitive_recursive_function.h"

/** @brief Composición h = f ∘ (g1, ..., gm). */
class Composition : public PrimitiveRecursiveFunction {
 public:
  /**
   * @param outer_function Función f, de aridad m.
   * @param combination Combinación (g1, ..., gm) de m funciones de aridad n
   * (que será la de la composición).
   * @throw InvalidFunctionDefinitionError Si f es nula o su aridad no es m.
   */
  Composition(FunctionPointer outer_function, Combination combination);

 protected:
  /** @brief Evalúa la combinación sobre arguments y entrega la tupla a f. */
  Natural Compute(const Arguments& arguments,
                  CallCounter& call_counter) const override;

 private:
  FunctionPointer outer_function_;  // f
  Combination combination_;         // (g1, ..., gm)
};

#endif  // COMPOSITION_H_