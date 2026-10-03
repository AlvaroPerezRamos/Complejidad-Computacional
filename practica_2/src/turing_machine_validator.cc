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
 * @file turing_machine_validator.cc
 * @brief Implementación de la clase TuringMachineValidator.
 */

#include "../include/turing_machine_validator.h"

void TuringMachineValidator::Validate(const TuringMachine& machine,
                                      std::ostream& warnings_stream) {
  const std::set<State> reachable_states = machine.ComputeReachableStates();

  WarnIfNoTransitions(machine, warnings_stream);
  WarnUnreachableStates(machine, reachable_states, warnings_stream);
  WarnIfNoFinalStateReachable(machine, reachable_states, warnings_stream);
}

void TuringMachineValidator::WarnIfNoTransitions(const TuringMachine& machine,
                                                 std::ostream& warnings_stream) {
  if (machine.GetTransitionFunction().Size() == 0) {
    warnings_stream << "[Aviso] La máquina no tiene ninguna transición: se detendrá "
        "inmediatamente en q0.\n";
  }
}

void TuringMachineValidator::WarnUnreachableStates(const TuringMachine& machine,
                                                   const std::set<State>& reachable_states,
                                                   std::ostream& warnings_stream) {
  for (const State& state : machine.GetStateDeclarationOrder()) {
    if (reachable_states.count(state) == 0) {
      warnings_stream << "[Aviso] El estado '" << state.GetName() << "' es inalcanzable desde q0.\n";
    }
  }
}

void TuringMachineValidator::WarnIfNoFinalStateReachable(const TuringMachine& machine,
                                                         const std::set<State>& reachable_states,
                                                         std::ostream& warnings_stream) {
  for (const State& final_state : machine.GetFinalStates()) {
    if (reachable_states.count(final_state) > 0) {
      return;
    }
  }
  warnings_stream << "[Aviso] Ningún estado final es alcanzable desde q0: ninguna cadena se "
      "aceptará nunca.\n";
}
