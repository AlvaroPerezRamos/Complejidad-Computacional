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
 * @file turing_configuration.cc
 * @brief Implementación de la clase TuringConfiguration.
 */

#include "../include/turing_configuration.h"

std::vector<Symbol> TuringConfiguration::ReadSymbols() const {
  std::vector<Symbol> read_symbols;
  read_symbols.reserve(tapes_.size());
  for (const Tape& tape : tapes_) {
    read_symbols.push_back(tape.Read());
  }
  return read_symbols;
}

void TuringConfiguration::ApplyTransition(const Transition& transition) {
  const std::vector<TapeAction>& tape_actions = transition.GetTapeActions();
  for (std::size_t tape_index = 0; tape_index < tapes_.size(); ++tape_index) {
    tapes_[tape_index].Write(tape_actions[tape_index].write_symbol);
    tapes_[tape_index].Move(tape_actions[tape_index].movement);
  }
  current_state_ = transition.GetDestinationState();
}
