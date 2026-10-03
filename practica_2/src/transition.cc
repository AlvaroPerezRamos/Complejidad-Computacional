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
 * @file transition.cc
 * @brief Implementación de la clase Transition.
 */

#include "../include/transition.h"

namespace {

char MovementToChar(Movement movement) {
  switch (movement) {
    case Movement::kLeft: return 'L';
    case Movement::kRight: return 'R';
    case Movement::kStay: return 'S';
  }
  return '?';  // Inalcanzable: Movement no tiene más valores.
}

}  // namespace

TransitionKey Transition::GetKey() const {
  std::vector<Symbol> read_symbols;
  read_symbols.reserve(tape_actions_.size());
  for (const TapeAction& action : tape_actions_) {
    read_symbols.push_back(action.read_symbol);
  }
  return TransitionKey{origin_state_, read_symbols};
}

std::string Transition::ToString() const {
  std::string read_part, write_part, movement_part;
  for (std::size_t i = 0; i < tape_actions_.size(); ++i) {
    if (i > 0) {
      read_part += ' ';
      write_part += ' ';
      movement_part += ' ';
    }
    read_part += tape_actions_[i].read_symbol.ToString();
    write_part += tape_actions_[i].write_symbol.ToString();
    movement_part += MovementToChar(tape_actions_[i].movement);
  }
  return "δ(" + origin_state_.GetName() + ", " + read_part + ") = (" +
      destination_state_.GetName() + ", " + write_part + ", " + movement_part + ")";
}

std::ostream& operator<<(std::ostream& output_stream, const Transition& transition) {
  output_stream << transition.ToString();
  return output_stream;
}
