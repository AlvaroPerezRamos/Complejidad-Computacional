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
 * @file transition_function.cc
 * @brief Implementación de la clase TransitionFunction.
 */

#include "../include/transition_function.h"

#include "../include/errors.h"

bool TransitionFunction::Insert(const Transition& transition) {
  const TransitionKey key = transition.GetKey();
  const auto found_entry = transitions_by_key_.find(key);

  if (found_entry == transitions_by_key_.end()) {
    transitions_by_key_.emplace(key, transition);
    return true;
  }
  if (found_entry->second == transition) {
    return false;  // Misma quíntupla repetida: aviso, se ignora.
  }
  throw NonDeterministicTransitionError(
      "Ya existe una transición distinta para (" + key.origin_state.GetName() +
      ", " + transition.ToString() + "): el fichero describe una máquina no determinista.");
}

std::optional<Transition> TransitionFunction::GetTransition(
    const State& origin_state, const std::vector<Symbol>& read_symbols) const {
  const auto found_entry = transitions_by_key_.find(TransitionKey{origin_state, read_symbols});
  if (found_entry == transitions_by_key_.end()) {
    return std::nullopt;
  }
  return found_entry->second;
}

std::vector<Transition> TransitionFunction::GetTransitionsFrom(const State& origin_state) const {
  std::vector<Transition> transitions_from_state;
  for (const auto& [key, transition] : transitions_by_key_) {
    if (transition.GetOriginState() == origin_state) {
      transitions_from_state.push_back(transition);
    }
  }
  return transitions_from_state;
}
