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
 * @file turing_machine.cc
 * @brief Implementación de la clase TuringMachine.
 */

#include "../include/turing_machine.h"

TuringMachine::TuringMachine(const std::vector<State>& state_declaration_order,
                             const Alphabet& input_alphabet,
                             const Alphabet& tape_alphabet,
                             const State& initial_state,
                             const Symbol& blank_symbol,
                             const std::vector<State>& final_states,
                             int tape_count,
                             const TransitionFunction& transition_function)
    : states_(state_declaration_order.begin(), state_declaration_order.end()),
      state_declaration_order_(state_declaration_order),
      input_alphabet_(input_alphabet),
      tape_alphabet_(tape_alphabet),
      initial_state_(initial_state),
      blank_symbol_(blank_symbol),
      final_states_(final_states.begin(), final_states.end()),
      tape_count_(tape_count),
      transition_function_(transition_function) {}

std::set<State> TuringMachine::ComputeReachableStates() const {
  std::set<State> reachable_states;
  std::vector<State> pending_states{initial_state_};
  reachable_states.insert(initial_state_);

  while (!pending_states.empty()) {
    const State current_state = pending_states.back();
    pending_states.pop_back();
    for (const Transition& transition : transition_function_.GetTransitionsFrom(current_state)) {
      const State& destination_state = transition.GetDestinationState();
      if (reachable_states.insert(destination_state).second) {
        pending_states.push_back(destination_state);
      }
    }
  }
  return reachable_states;
}
