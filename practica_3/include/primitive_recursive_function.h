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
 * @brief Definición de la clase abstracta PrimitiveRecursiveFunction, raíz
 * de la jerarquía: toda función (básica, compuesta o definida por recursión)
 * es una función ℕⁿ → ℕ con una aridad n fija.
 *
 * Patrón "método plantilla": Evaluate() es público y NO virtual; comprueba la
 * aridad, anota la llamada en el CallCounter (si la función cuenta como
 * llamada) y delega el cálculo en el método virtual protegido Compute(). Así
 * es imposible que una subclase se olvide de contar su llamada o de validar
 * los argumentos.
 *
 * Qué cuenta como llamada: las funciones básicas (Z, S, Pᵢⁿ) y las funciones
 * con nombre (uno, suma, producto, potencia: clase NamedFunction), cada vez
 * que se entra en ellas. Las operaciones (combinación, composición y recursión
 * primitiva) no cuentan por sí mismas: de la recursión solo cuentan sus
 * llamadas recursivas.
 */

#ifndef PRIMITIVE_RECURSIVE_FUNCTION_H_
#define PRIMITIVE_RECURSIVE_FUNCTION_H_

#include <cstddef>
#include <memory>
#include <string>

#include "call_counter.h"
#include "types.h"

/** @brief Función ℕⁿ → ℕ primitiva recursiva, evaluable y con aridad conocida.
 */
class PrimitiveRecursiveFunction {
 public:
  /** @brief Destructor virtual: los objetos se destruyen a través de punteros a
   * la base. */
  virtual ~PrimitiveRecursiveFunction() = default;

  /**
   * @brief Evalúa la función en una tupla de argumentos.
   * @param arguments Tupla (x1, ..., xn); debe tener exactamente GetArity()
   * elementos.
   * @param call_counter Contador donde se anota esta llamada (si cuenta como
   * llamada) y todas las que ésta provoque.
   * @return El valor de la función.
   * @throw ArityMismatchError Si arguments.size() != GetArity().
   * @throw NaturalOverflowError Si algún valor intermedio no cabe en Natural
   * (lo lanza SuccessorFunction).
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
   * @param counts_call Si cada Evaluate() anota una llamada en el contador.
   * Es true para las funciones básicas y las funciones con nombre, y false
   * para las operaciones (composición y recursión primitiva).
   */
  PrimitiveRecursiveFunction(std::string name, std::size_t arity,
                             bool counts_call = true);

  /**
   * @brief Calcula el valor de la función. Lo implementa cada subclase.
   * @param arguments Argumentos ya validados (tamaño == GetArity()).
   * @param call_counter Contador que debe propagarse a las sub-evaluaciones.
   */
  virtual Natural Compute(const Arguments& arguments,
                          CallCounter& call_counter) const = 0;

 private:
  std::string name_;
  std::size_t arity_;
  bool counts_call_;
};

/** @brief Las funciones son inmutables una vez construidas y se comparten por
 * composición. */
using FunctionPointer = std::shared_ptr<const PrimitiveRecursiveFunction>;

#endif  // PRIMITIVE_RECURSIVE_FUNCTION_H_