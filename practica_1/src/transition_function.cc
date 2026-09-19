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
 * @file transition_function.cc
 * @brief Implementación de la clase TransitionFunction.
 *
 * Historial de versiones
 *   19/09/2026 - Creación e implementación completa.
 */

#include "../include/transition_function.h"

#include <algorithm>

bool TransitionFunction::Insert(const Transition& transition) {
  std::vector<Transition>& transitions_with_same_key =
      transitions_by_key_[transition.GetKey()];
  for (const Transition& existing_transition : transitions_with_same_key) {
    if (existing_transition == transition) {
      return false;  // Ya existía exactamente esta misma quíntupla.
    }
  }
  transitions_with_same_key.push_back(transition);
  insertion_order_.push_back(transition);
  return true;
}

std::vector<Transition> TransitionFunction::GetApplicableTransitions(
    const State& origin_state, const Symbol& input_symbol,
    const Symbol& stack_symbol) const {
  const TransitionKey key{origin_state, input_symbol, stack_symbol};
  const auto found_entry = transitions_by_key_.find(key);
  return found_entry == transitions_by_key_.end() ? std::vector<Transition>{}
                                                  : found_entry->second;
}

std::vector<Transition> TransitionFunction::GetTransitionsFrom(
    const State& origin_state) const {
  std::vector<Transition> transitions_from_state;
  for (const Transition& transition : insertion_order_) {
    if (transition.GetOriginState() == origin_state) {
      transitions_from_state.push_back(transition);
    }
  }
  return transitions_from_state;
}

std::vector<Transition> TransitionFunction::GetOrderedTransitions(
    const std::vector<State>& state_declaration_order) const {
  std::vector<Transition> ordered_transitions;
  for (const State& origin_state : state_declaration_order) {
    std::vector<Transition> transitions_from_state =
        GetTransitionsFrom(origin_state);
    // Separa, sin reordenar dentro de cada grupo, los autobucles del resto.
    std::stable_partition(
        transitions_from_state.begin(), transitions_from_state.end(),
        [&origin_state](const Transition& transition) {
          return transition.GetDestinationState() == origin_state;
        });
    ordered_transitions.insert(ordered_transitions.end(),
                               transitions_from_state.begin(),
                               transitions_from_state.end());
  }
  return ordered_transitions;
}