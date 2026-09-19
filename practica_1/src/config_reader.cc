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
 * @file config_reader.cc
 * @brief Implementación de las funciones de config_reader.h.
 *
 * Historial de versiones
 *   19/09/2026 - Creación del fichero: se extrae sin cambios la
 *                implementación que ya estaba en main.cc.
 */

#include "../include/config_reader.h"

#include <sstream>

#include "../include/errors.h"

std::string StripComment(const std::string& line) {
  const std::size_t comment_position = line.find('#');
  return comment_position == std::string::npos
             ? line
             : line.substr(0, comment_position);
}

bool IsBlankLine(const std::string& line) {
  return line.find_first_not_of(" \t\r\n") == std::string::npos;
}

bool ReadNextSignificantLine(std::ifstream& config_file,
                             std::string& significant_line) {
  std::string raw_line;
  while (std::getline(config_file, raw_line)) {
    const std::string stripped_line = StripComment(raw_line);
    if (!IsBlankLine(stripped_line)) {
      significant_line = stripped_line;
      return true;
    }
  }
  return false;
}

std::vector<std::string> SplitIntoTokens(const std::string& line) {
  std::vector<std::string> tokens;
  std::istringstream token_stream(line);
  std::string token;
  while (token_stream >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

std::vector<State> ParseStates(const std::string& states_line) {
  std::vector<State> states;
  for (const std::string& token : SplitIntoTokens(states_line)) {
    states.emplace_back(token);
  }
  return states;
}

State ParseSingleState(const std::string& line) {
  const std::vector<std::string> tokens = SplitIntoTokens(line);
  if (tokens.size() != 1) {
    throw MissingSectionError(
        "El estado inicial (q0) debe ser exactamente "
        "un estado; se han encontrado " +
        std::to_string(tokens.size()) + ".");
  }
  return State(tokens.front());
}

Symbol ParseSingleSymbol(const std::string& line) {
  const std::vector<std::string> tokens = SplitIntoTokens(line);
  if (tokens.size() != 1) {
    throw MissingSectionError(
        "El símbolo inicial de la pila (Z0) debe ser "
        "exactamente un símbolo; se han encontrado " +
        std::to_string(tokens.size()) + ".");
  }
  if (tokens.front().length() != 1) {
    throw InvalidSymbolError("El símbolo inicial de la pila '" +
                             tokens.front() +
                             "' no es válido: debe tener un único carácter.");
  }
  return Symbol(tokens.front()[0]);
}

Transition ParseTransitionLine(const std::string& transition_line) {
  const std::vector<std::string> tokens = SplitIntoTokens(transition_line);
  if (tokens.size() != 5) {
    throw InvalidTransitionError(
        "La transición '" + transition_line +
        "' no es válida: debe tener exactamente 5 campos (origen, entrada, "
        "cima, destino, símbolos a apilar); tiene " +
        std::to_string(tokens.size()) + ".");
  }

  const std::string& origin_token = tokens[0];
  const std::string& input_token = tokens[1];
  const std::string& stack_top_token = tokens[2];
  const std::string& destination_token = tokens[3];
  const std::string& push_token = tokens[4];

  if (input_token.length() != 1) {
    throw InvalidTransitionError(
        "El símbolo de entrada '" + input_token + "' de la transición '" +
        transition_line + "' no es válido: debe tener un único carácter.");
  }
  if (stack_top_token.length() != 1) {
    throw InvalidTransitionError(
        "El símbolo de la cima '" + stack_top_token + "' de la transición '" +
        transition_line + "' no es válido: debe tener un único carácter.");
  }
  if (stack_top_token[0] == kEpsilonCharacter) {
    throw InvalidTransitionError(
        "La transición '" + transition_line +
        "' no es válida: una transición nunca puede consultar ε en la cima "
        "de la pila.");
  }

  const State origin_state(origin_token);
  const Symbol input_symbol(input_token[0]);
  const Symbol stack_symbol(stack_top_token[0]);
  const State destination_state(destination_token);
  // '.' representa "no apilar nada": se normaliza a cadena vacía, que es
  // el convenio que ya entienden Stack::Push() y Transition::ToString().
  const std::string symbols_to_push =
      push_token == std::string(1, kEpsilonCharacter) ? "" : push_token;

  return Transition(origin_state, input_symbol, stack_symbol, destination_state,
                    symbols_to_push);
}