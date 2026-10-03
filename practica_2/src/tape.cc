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
 * @file tape.cc
 * @brief Implementación de la clase Tape.
 */

#include "../include/tape.h"

#include <algorithm>

Tape::Tape(const Symbol& blank_symbol, const std::string& initial_content)
    : blank_symbol_(blank_symbol) {
  for (char character : initial_content) {
    Write(Symbol(character));
    ++head_position_;
  }
  head_position_ = 0;
}

Symbol Tape::SymbolAt(long long position) const {
  const auto found_cell = cells_.find(position);
  return found_cell == cells_.end() ? blank_symbol_ : found_cell->second;
}

void Tape::Write(const Symbol& symbol) {
  if (symbol == blank_symbol_) {
    cells_.erase(head_position_);  // Mantiene el mapa disperso.
  } else {
    // insert_or_assign (no operator[]): Symbol no tiene constructor por
    // defecto, y operator[] lo necesitaría para la clave nueva.
    cells_.insert_or_assign(head_position_, symbol);
  }
}

void Tape::Move(Movement movement) {
  switch (movement) {
    case Movement::kLeft:
      --head_position_;
      break;
    case Movement::kRight:
      ++head_position_;
      break;
    case Movement::kStay:
      break;
  }
}

std::string Tape::ToString() const {
  long long min_position = head_position_;
  long long max_position = head_position_;
  if (!cells_.empty()) {
    min_position = std::min(min_position, cells_.begin()->first);
    max_position = std::max(max_position, cells_.rbegin()->first);
  }

  std::string result;
  for (long long position = min_position; position <= max_position; ++position) {
    if (position == head_position_) result += '[';
    result += SymbolAt(position).GetCharacter();
    if (position == head_position_) result += ']';
  }
  return result;
}
