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
 * @file primitive_recursive_function.h
 * @brief Definición de la clase abstracta PrimitiveRecursiveFunction.
 *
 * Es la raíz de la jerarquía. Todas las funciones (básicas, compuestas y
 * definidas por recursión) son una función ℕⁿ → ℕ con una aridad n fija.
 *
 * Patrón "método plantilla": Evaluate() es público y NO virtual; comprueba la
 * aridad, anota la llamada en el CallCounter y delega el cálculo en el método
 * virtual protegido Compute(). Así es imposible que una subclase se olvide de
 * contar su llamada o de validar los argumentos.
 */

#ifndef PRIMITIVE_RECURSIVE_FUNCTION_H_
#define PRIMITIVE_RECURSIVE_FUNCTION_H_

#include <cstddef>
#include <memory>
#include <string>

#include "call_counter.h"
#include "errors.h"
#include "types.h"

/**
 * @class PrimitiveRecursiveFunction
 * @brief Función ℕⁿ → ℕ primitiva recursiva, evaluable y con aridad conocida.
 */
class PrimitiveRecursiveFunction {
 public:
  /** @brief Destructor virtual: se destruyen a través de punteros a la base. */
  virtual ~PrimitiveRecursiveFunction() = default;

  /**
   * @brief Evalúa la función en una tupla de argumentos.
   * @param arguments Tupla (x1, ..., xn); debe tener exactamente GetArity()
   * elementos.
   * @param call_counter Contador donde se anota esta llamada y todas las que
   * ésta provoque.
   * @return El valor de la función.
   * @throw ArityMismatchError Si arguments.size() != GetArity().
   * @throw NaturalOverflowError Si algún valor intermedio no cabe en Natural.
   */
  Natural Evaluate(const Arguments& arguments, CallCounter& call_counter) const;

  /** @brief Devuelve la aridad n de la función (ℕⁿ → ℕ). */
  std::size_t GetArity() const { return arity_; }

  /** @brief Devuelve un nombre legible, p. ej. "S∘Z" o "P_1^3". */
  const std::string& GetName() const { return name_; }

 protected:
  /**
   * @brief Construye la parte común de cualquier función.
   * @param name Nombre legible de la función.
   * @param arity Número de argumentos que espera.
   */
  PrimitiveRecursiveFunction(std::string name, std::size_t arity);

  /**
   * @brief Calcula el valor de la función. Lo implementa cada subclase.
   * @param arguments Argumentos ya validados (tamaño == GetArity()).
   * @param call_counter Contador que debe propagarse a las sub-evaluaciones.
   */
  virtual Natural Compute(const Arguments& arguments,
                          CallCounter& call_counter) const = 0;

 private:
  std::string name_;  /**< Nombre legible. */
  std::size_t arity_; /**< Aridad n. */
};

/** Las funciones son inmutables una vez construidas, y se comparten por
 * composición. */
using FunctionPointer = std::shared_ptr<const PrimitiveRecursiveFunction>;

#endif  // PRIMITIVE_RECURSIVE_FUNCTION_H_
