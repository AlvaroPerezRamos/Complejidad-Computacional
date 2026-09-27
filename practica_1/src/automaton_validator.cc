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
 * @file automaton_validator.cc
 * @brief Implementación de la clase AutomatonValidator.
 */

#include "../include/automaton_validator.h"
#include "../include/symbol.h"
#include "../include/transition.h"

void AutomatonValidator::Validate(const PushdownAutomaton& automaton,
                                  std::ostream& warnings_stream) {
  const std::set<State> reachable_states = automaton.ComputeReachableStates();

  WarnIfNoTransitions(automaton, warnings_stream);
  WarnUnreachableStates(automaton, reachable_states, warnings_stream);
  WarnIfNoFinalStateReachable(automaton, reachable_states, warnings_stream);
  WarnIfAlphabetsOverlap(automaton, warnings_stream);
  WarnIfNoTransitionAtStart(automaton, warnings_stream);
}

void AutomatonValidator::WarnIfNoTransitions(const PushdownAutomaton& automaton,
                                             std::ostream& warnings_stream) {
  if (automaton.GetTransitionFunction().Size() == 0) {
    warnings_stream << "[Aviso] El autómata no tiene ninguna transición: solo "
                       "podría aceptar la cadena vacía.\n";
  }
}

void AutomatonValidator::WarnUnreachableStates(
    const PushdownAutomaton& automaton, const std::set<State>& reachable_states,
    std::ostream& warnings_stream) {
  for (const State& state : automaton.GetStateDeclarationOrder()) {
    if (reachable_states.count(state) == 0) {
      warnings_stream << "[Aviso] El estado '" << state.GetName()
                      << "' es inalcanzable desde q0.\n";
    }
  }
}

void AutomatonValidator::WarnIfNoFinalStateReachable(
    const PushdownAutomaton& automaton, const std::set<State>& reachable_states,
    std::ostream& warnings_stream) {
  for (const State& final_state : automaton.GetFinalStates()) {
    if (reachable_states.count(final_state) > 0) {
      return;  // Hay al menos un estado final alcanzable: no hace falta avisar.
    }
  }
  warnings_stream << "[Aviso] Ningún estado final es alcanzable desde q0: el "
                     "lenguaje reconocido es vacío.\n";
}

void AutomatonValidator::WarnIfAlphabetsOverlap(
    const PushdownAutomaton& automaton, std::ostream& warnings_stream) {
  std::set<Symbol> shared_symbols;
  for (const Symbol& symbol : automaton.GetInputAlphabet().GetAlphabet()) {
    if (automaton.GetStackAlphabet().Contains(symbol)) {
      shared_symbols.insert(symbol);
    }
  }
  if (shared_symbols.empty()) {
    return;
  }

  warnings_stream << "[Aviso] Σ ∩ Γ ≠ ∅: comparten el símbolo"
                  << (shared_symbols.size() == 1 ? " " : "s ");
  bool is_first_symbol = true;
  for (const Symbol& symbol : shared_symbols) {
    if (!is_first_symbol) warnings_stream << ", ";
    warnings_stream << "'" << symbol << "'";
    is_first_symbol = false;
  }
  warnings_stream << ".\n";
}

void AutomatonValidator::WarnIfNoTransitionAtStart(
    const PushdownAutomaton& automaton, std::ostream& warnings_stream) {
  const auto& transitions_from_initial_state =
      automaton.GetTransitionFunction().GetTransitionsFrom(
          automaton.GetInitialState());
  for (const Transition& transition : transitions_from_initial_state) {
    if (transition.GetStackSymbol() == automaton.GetInitialStackSymbol()) {
      return;  // Hay al menos una transición aplicable en (q0, Z0).
    }
  }
  warnings_stream << "[Aviso] No hay ninguna transición aplicable en la "
                     "descripción instantánea inicial (q0, Z0): el autómata se "
                     "detiene nada "
                     "más arrancar.\n";
}