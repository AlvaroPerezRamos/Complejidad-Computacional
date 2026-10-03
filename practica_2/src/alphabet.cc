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
 * @file alphabet.cc
 * @brief Implementación de la clase Alphabet.
 */

#include "../include/alphabet.h"

#include <sstream>

#include "../include/errors.h"

Alphabet::Alphabet(const std::string& symbols_line) {
  std::istringstream token_stream(symbols_line);
  std::string token;
  while (token_stream >> token) {
    ValidateSymbolToken(token);
    if (!InsertSymbol(Symbol(token[0]))) {
      throw DuplicatedElementError("El símbolo '" + token +
          "' aparece repetido en el alfabeto.");
    }
  }
  if (IsEmpty()) {
    throw MissingSectionError("El alfabeto no puede estar vacío.");
  }
}

void Alphabet::ValidateSymbolToken(const std::string& token) const {
  if (token.length() != 1) {
    throw InvalidSymbolError("El símbolo '" + token + "' no es válido: los "
        "símbolos de un alfabeto deben tener exactamente un carácter.");
  }
}

bool Alphabet::InsertSymbol(const Symbol& symbol) {
  return symbols_.insert(symbol).second;
}

std::ostream& operator<<(std::ostream& output_stream, const Alphabet& alphabet) {
  output_stream << "{";
  bool is_first_symbol = true;
  for (const Symbol& symbol : alphabet.symbols_) {
    if (!is_first_symbol) output_stream << ", ";
    output_stream << symbol;
    is_first_symbol = false;
  }
  output_stream << "}";
  return output_stream;
}
