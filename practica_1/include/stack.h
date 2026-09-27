/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 19/09/2026
 * @file stack.h
 * @brief Definición de la clase Stack. Se representa como un string; la
 * posición 0 es la cima.
 */

#ifndef STACK_H_
#define STACK_H_

#include <cstddef>
#include <iostream>
#include <string>

#include "symbol.h"

/**
 * @class Stack
 * @brief Representa la pila de un autómata con pila.
 */
class Stack {
 public:
  /** @brief Construye una pila vacía. */
  Stack() = default;

  /**
   * @brief Construye una pila con un único símbolo, pensada para
   * inicializarla directamente con el símbolo inicial de la pila (Z0).
   * @param initial_symbol Símbolo que queda como única entrada de la pila.
   */
  explicit Stack(const Symbol& initial_symbol) : symbols_(initial_symbol.ToString()) {}

  /** @brief Destructor por defecto. */
  ~Stack() = default;

  /**
   * @brief Devuelve el símbolo de la cima de la pila.
   * @throw EmptyStackError Si la pila está vacía.
   */
  Symbol Top() const;

  /**
   * @brief Elimina el símbolo de la cima de la pila.
   * @throw EmptyStackError Si la pila está vacía.
   */
  void Pop();

  /**
   * @brief Apila una secuencia de símbolos encima de la cima actual.
   * @param symbols_to_push Secuencia a apilar, en el mismo orden que en el
   * fichero de configuración (el primer carácter queda en la cima). Una
   * cadena vacía no modifica la pila.
   */
  void Push(const std::string& symbols_to_push);

  /** @brief Indica si la pila no contiene ningún símbolo. */
  bool IsEmpty() const { return symbols_.empty(); }

  /** @brief Devuelve el número de símbolos de la pila. */
  std::size_t Size() const { return symbols_.size(); }

  /** @brief Operador de igualdad entre pilas (mismo contenido, en el mismo orden). */
  bool operator==(const Stack& other) const { return symbols_ == other.symbols_; }

  /** @brief Operador de desigualdad entre pilas. */
  bool operator!=(const Stack& other) const { return !(*this == other); }

  /**
   * @brief Orden total arbitrario entre pilas (lexicográfico sobre su
   * contenido). No tiene ningún significado en la teoría de autómatas:
   * existe solo para poder usar Stack (y, con ella,
   * InstantaneousDescription) como clave de un std::set.
   */
  bool operator<(const Stack& other) const { return symbols_ < other.symbols_; }

  /** @brief Sobrecarga del operador de salida: imprime la pila de la cima al fondo. */
  friend std::ostream& operator<<(std::ostream& output_stream, const Stack& stack);

 private:
  std::string symbols_;  /**< Contenido de la pila; symbols_[0] es la cima. */
};

#endif  // STACK_H_