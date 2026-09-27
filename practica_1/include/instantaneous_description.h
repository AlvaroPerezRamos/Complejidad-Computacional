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
 * @brief Definición de InstantaneousDescription: la terna (q, w, α).
 * Inmutable; Simulator construye una nueva por cada transición aplicada.
 */

#ifndef INSTANTANEOUS_DESCRIPTION_H_
#define INSTANTANEOUS_DESCRIPTION_H_

#include <string>
#include <tuple>

#include "stack.h"
#include "state.h"
#include "symbol.h"
#include "errors.h"

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
  Symbol GetNextInputSymbol() const {
    if (IsInputConsumed()) {
      throw SimulationError(
          "No se puede consultar el siguiente símbolo de "
          "entrada: la entrada ya está consumida.");
    }
    return Symbol(remaining_input_.front());
  }

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