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
 * @file pushdown_automaton.cc
 * @brief Implementación de la clase PushdownAutomaton.
 */

#include <vector>

#include "../include/pushdown_automaton.h"

PushdownAutomaton::PushdownAutomaton(
    const std::vector<State>& state_declaration_order,
    const Alphabet& input_alphabet, const Alphabet& stack_alphabet,
    const State& initial_state, const Symbol& initial_stack_symbol,
    const std::vector<State>& final_states,
    const TransitionFunction& transition_function)
    : states_(state_declaration_order.begin(), state_declaration_order.end()),
      state_declaration_order_(state_declaration_order),
      input_alphabet_(input_alphabet),
      stack_alphabet_(stack_alphabet),
      initial_state_(initial_state),
      initial_stack_symbol_(initial_stack_symbol),
      final_states_(final_states.begin(), final_states.end()),
      transition_function_(transition_function) {}

std::set<State> PushdownAutomaton::ComputeReachableStates() const {
  std::set<State> reachable_states;
  std::vector<State> pending_states{initial_state_};
  reachable_states.insert(initial_state_);

  while (!pending_states.empty()) {
    const State current_state = pending_states.back();
    pending_states.pop_back();
    for (const Transition& transition :
         transition_function_.GetTransitionsFrom(current_state)) {
      const State& destination_state = transition.GetDestinationState();
      if (reachable_states.insert(destination_state).second) {
        pending_states.push_back(destination_state);
      }
    }
  }
  return reachable_states;
}