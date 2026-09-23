/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 23/09/2026
 * @file instantaneous_description.h
 * @brief Definición de la clase InstantaneousDescription.
 *
 * Representa la terna (q, w, α) de la sección 5 del enunciado: el estado
 * actual, la cadena de entrada todavía pendiente de consumir, y la pila
 * en ese instante de la computación. Es inmutable: Simulator no modifica
 * una InstantaneousDescription existente, construye una nueva a partir de
 * ella cada vez que aplica una transición (así cada rama de la búsqueda
 * en profundidad conserva su propia copia, sin interferir con las demás).
 *
 * No guarda una Chain como entrada, sino un std::string ya "en curso":
 * Chain solo tiene sentido para la cadena completa, validada de una vez
 * contra Σ al principio; aquí lo que importa es ir consumiendo esa cadena
 * carácter a carácter, sin volver a validar nada en cada paso.
 *
 * operator==() y operator<() existen porque el algoritmo de la sección 5
 * los necesita: la salvaguarda "descripciones repetidas en la rama
 * actual" (para podar los ciclos de ε-transiciones) exige poder comparar
 * dos descripciones instantáneas y guardar las ya visitadas en un
 * std::set<InstantaneousDescription>, lo que a su vez exige que Stack
 * tenga su propio operator< (añadido ahora en stack.h con este único
 * propósito: no tiene ningún significado en la teoría de autómatas, solo
 * sirve para poder ordenar).
 *
 * Historial de versiones
 *   23/09/2026 - Creación del fichero e implementación completa.
 */

#ifndef INSTANTANEOUS_DESCRIPTION_H_
#define INSTANTANEOUS_DESCRIPTION_H_

#include <string>
#include <tuple>

#include "stack.h"
#include "state.h"
#include "symbol.h"

/**
 * @class InstantaneousDescription
 * @brief Representa la terna (q, w, α): estado, cadena de entrada
 * pendiente y pila, en un instante concreto de la computación.
 */
class InstantaneousDescription {
 public:
  /**
   * @brief Construye una descripción instantánea.
   * @param state Estado actual (q).
   * @param remaining_input Cadena de entrada todavía pendiente de
   * consumir (w).
   * @param stack Pila en este instante (α).
   */
  InstantaneousDescription(const State& state, const std::string& remaining_input,
                            const Stack& stack)
      : state_(state), remaining_input_(remaining_input), stack_(stack) {}

  /** @brief Destructor por defecto. */
  ~InstantaneousDescription() = default;

  /** @brief Devuelve el estado actual (q). */
  const State& GetState() const { return state_; }

  /** @brief Devuelve la cadena de entrada todavía pendiente de consumir (w). */
  const std::string& GetRemainingInput() const { return remaining_input_; }

  /** @brief Devuelve la pila en este instante (α). */
  const Stack& GetStack() const { return stack_; }

  /** @brief Indica si ya no queda entrada pendiente (w = ε). */
  bool IsInputConsumed() const { return remaining_input_.empty(); }

  /**
   * @brief Devuelve el primer símbolo de la entrada todavía pendiente.
   * @throw SimulationError Si la entrada ya está consumida: es una
   * salvaguarda defensiva, igual que Stack::Top() con EmptyStackError.
   * Simulator debe comprobar IsInputConsumed() antes de llamar a este
   * método, no dejar que este método detecte el problema.
   */
  Symbol GetNextInputSymbol() const;

  /** @brief Operador de igualdad: mismo estado, misma entrada pendiente y misma pila. */
  bool operator==(const InstantaneousDescription& other) const {
    return state_ == other.state_ && remaining_input_ == other.remaining_input_ &&
           stack_ == other.stack_;
  }

  /** @brief Operador de desigualdad. */
  bool operator!=(const InstantaneousDescription& other) const { return !(*this == other); }

  /**
   * @brief Orden total arbitrario, necesario para guardar descripciones
   * ya visitadas en un std::set (detección de ciclos de la sección 5).
   */
  bool operator<(const InstantaneousDescription& other) const {
    return std::tie(state_, remaining_input_, stack_) <
           std::tie(other.state_, other.remaining_input_, other.stack_);
  }

 private:
  State state_;                /**< Estado actual (q). */
  std::string remaining_input_;  /**< Cadena de entrada pendiente (w). */
  Stack stack_;                 /**< Pila en este instante (α). */
};

#endif  // INSTANTANEOUS_DESCRIPTION_H_