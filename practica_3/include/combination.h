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
 * @file combination.h
 * @brief Definición de la clase Combination: la operación de combinación.
 * Dadas g1, ..., gm : ℕⁿ → ℕ, su combinación es la función
 * (g1, ..., gm) : ℕⁿ → ℕᵐ que a x le asigna la TUPLA (g1(x), ..., gm(x)).
 *
 * A diferencia de Z, S, Pᵢⁿ, la composición y la recursión, la combinación
 * devuelve una tupla y no un único natural, así que NO es una
 * PrimitiveRecursiveFunction. La usa Composition: f ∘ (g1, ..., gm) es
 * "combinar y después aplicar f".
 *
 * Contabilidad de llamadas: la combinación no anota llamada propia, solo las
 * de las gi que evalúa. Se cuentan invocaciones de funciones de ℕⁿ → ℕ.
 */

#ifndef COMBINATION_H_
#define COMBINATION_H_

#include <cstddef>
#include <string>
#include <vector>

#include "primitive_recursive_function.h"

/** @brief Combinación (g1, ..., gm) de funciones con la misma aridad n. */
class Combination {
 public:
  /**
   * @param inner_functions Funciones g1..gm: al menos una, no nulas y todas
   * con la misma aridad n.
   * @throw InvalidFunctionDefinitionError Si algún requisito anterior falla.
   */
  explicit Combination(std::vector<FunctionPointer> inner_functions);

  /**
   * @brief Evalúa cada gi sobre arguments y devuelve la tupla de resultados,
   * en el mismo orden que las funciones.
   * @throw ArityMismatchError Si arguments no tiene n elementos (lo detecta la
   * primera gi evaluada).
   */
  Arguments Evaluate(const Arguments& arguments,
                     CallCounter& call_counter) const;

  /** @brief Aridad n común de las gi (la de los argumentos que acepta). */
  std::size_t GetArity() const;

  /** @brief Número m de funciones (el tamaño de la tupla que devuelve). */
  std::size_t GetSize() const;

  /** @brief Nombre legible: "(g1,...,gm)". */
  std::string GetName() const;

 private:
  std::vector<FunctionPointer> inner_functions_;  // g1, ..., gm
};

#endif  // COMBINATION_H_