/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 2: Simulador de una Máquina de Turing
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 03/10/2026
 * @file transition.h
 * @brief Definición de Transition y la clave TransitionKey. Generaliza la
 * quíntupla de P01 a N cintas: tape_actions_[i] son los datos de la
 * cinta i+1 (ver tape_action.h). No valida nada: es responsabilidad de
 * TuringMachineParser.
 */

#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <iostream>
#include <tuple>
#include <vector>

#include "state.h"
#include "tape_action.h"

/** @brief Clave de indexación en TransitionFunction: (origen, símbolos leídos en cada cinta). */
struct TransitionKey {
  State origin_state;
  std::vector<Symbol> read_symbols;

  bool operator<(const TransitionKey& other) const {
    return std::tie(origin_state, read_symbols) < std::tie(other.origin_state, other.read_symbols);
  }
};

/** @brief Una transición: (origen, símbolos leídos) -> (destino, símbolos escritos, movimientos), para N cintas. */
class Transition {
 public:
  Transition(const State& origin_state, const State& destination_state,
             const std::vector<TapeAction>& tape_actions)
      : origin_state_(origin_state),
        destination_state_(destination_state),
        tape_actions_(tape_actions) {}

  const State& GetOriginState() const { return origin_state_; }
  const State& GetDestinationState() const { return destination_state_; }
  const std::vector<TapeAction>& GetTapeActions() const { return tape_actions_; }

  /** @brief Clave bajo la que se indexa en TransitionFunction. */
  TransitionKey GetKey() const;

  std::string ToString() const;

  bool operator==(const Transition& other) const {
    return origin_state_ == other.origin_state_ &&
           destination_state_ == other.destination_state_ &&
           tape_actions_ == other.tape_actions_;
  }
  bool operator!=(const Transition& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& output_stream, const Transition& transition);

 private:
  State origin_state_;
  State destination_state_;
  std::vector<TapeAction> tape_actions_;
};

#endif  // TRANSITION_H_
