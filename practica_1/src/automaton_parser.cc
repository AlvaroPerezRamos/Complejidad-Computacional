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
 * @file automaton_parser.cc
 * @brief Implementación de la clase AutomatonParser.
 *
 * Historial de versiones
 *   19/09/2026 - Creación e implementación completa.
 */

#include <fstream>
#include <sstream>

#include "../include/automaton_parser.h"
#include "../include/errors.h"

std::string AutomatonParser::StripComment(const std::string& line) {
  const std::size_t comment_position = line.find('#');
  return comment_position == std::string::npos
             ? line
             : line.substr(0, comment_position);
}

bool AutomatonParser::IsBlankLine(const std::string& line) {
  return line.find_first_not_of(" \t\r\n") == std::string::npos;
}

std::vector<std::string> AutomatonParser::SplitIntoTokens(
    const std::string& line) {
  std::vector<std::string> tokens;
  std::istringstream token_stream(line);
  std::string token;
  while (token_stream >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

std::vector<AutomatonParser::SourceLine> AutomatonParser::ReadSignificantLines()
    const {
  std::ifstream config_file(configuration_file_path_);
  if (!config_file.is_open()) {
    throw FileError("No se puede abrir el fichero de configuración '" +
                    configuration_file_path_ + "'.");
  }

  std::vector<SourceLine> significant_lines;
  std::string raw_line;
  int line_number = 0;
  while (std::getline(config_file, raw_line)) {
    ++line_number;
    const std::string stripped_line = StripComment(raw_line);
    if (!IsBlankLine(stripped_line)) {
      significant_lines.push_back(SourceLine{line_number, stripped_line});
    }
  }

  if (significant_lines.empty()) {
    throw FileError("El fichero de configuración '" + configuration_file_path_ +
                    "' está vacío.");
  }
  return significant_lines;
}

bool AutomatonParser::TryGetNextLine(SourceLine& next_line) {
  if (current_line_index_ >= significant_lines_.size()) {
    return false;
  }
  next_line = significant_lines_[current_line_index_];
  ++current_line_index_;
  return true;
}

void AutomatonParser::CheckNoDuplicateStates(const std::vector<State>& states,
                                             const std::string& section_name,
                                             int line_number) const {
  std::set<State> seen_states;
  for (const State& state : states) {
    if (!seen_states.insert(state).second) {
      throw DuplicatedElementError("El estado '" + state.GetName() +
                                       "' aparece repetido en " + section_name +
                                       ".",
                                   line_number);
    }
  }
}

std::vector<State> AutomatonParser::ParseStatesSection(
    const std::string& section_name, bool allow_empty) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: falta "
        "la línea de " +
        section_name + ".");
  }

  std::vector<State> states;
  for (const std::string& token : SplitIntoTokens(current_line.content)) {
    states.emplace_back(token);
  }
  if (!allow_empty && states.empty()) {
    throw MissingSectionError(section_name + " no puede estar vacío.",
                              current_line.line_number);
  }
  CheckNoDuplicateStates(states, section_name, current_line.line_number);
  return states;
}

Alphabet AutomatonParser::ParseAlphabetSection(
    const std::string& section_name) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: falta "
        "la línea de " +
        section_name + ".");
  }
  try {
    return Alphabet(current_line.content);
  } catch (ConfigurationError& error) {
    error.SetLineNumber(current_line.line_number);
    throw;
  }
}

State AutomatonParser::ParseInitialStateSection(const std::set<State>& states) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: "
        "falta la línea de q0.");
  }
  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);
  if (tokens.size() != 1) {
    throw MissingSectionError(
        "El estado inicial (q0) debe ser exactamente un "
        "estado; se han encontrado " +
            std::to_string(tokens.size()) + ".",
        current_line.line_number);
  }
  const State initial_state(tokens.front());
  if (states.count(initial_state) == 0) {
    throw InvalidStateError(
        "El estado inicial '" + tokens.front() + "' (q0) no pertenece a Q.",
        current_line.line_number);
  }
  return initial_state;
}

Symbol AutomatonParser::ParseInitialStackSymbolSection(
    const Alphabet& stack_alphabet) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: "
        "falta la línea de Z0.");
  }
  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);
  if (tokens.size() != 1) {
    throw MissingSectionError(
        "El símbolo inicial de la pila (Z0) debe ser "
        "exactamente un símbolo; se han encontrado " +
            std::to_string(tokens.size()) + ".",
        current_line.line_number);
  }
  if (tokens.front().length() != 1) {
    throw InvalidSymbolError(
        "El símbolo inicial de la pila '" + tokens.front() +
            "' no es válido: debe tener un único carácter.",
        current_line.line_number);
  }
  const Symbol initial_stack_symbol(tokens.front()[0]);
  if (!stack_alphabet.Contains(initial_stack_symbol)) {
    throw InvalidSymbolError("El símbolo inicial de la pila '" +
                                 tokens.front() + "' (Z0) no pertenece a Γ.",
                             current_line.line_number);
  }
  return initial_stack_symbol;
}

std::vector<State> AutomatonParser::ParseFinalStatesSection(
    const std::set<State>& states) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: "
        "falta la línea de F.");
  }

  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);

  // Heurística: 5 tokens que no son todos estados ya declarados en Q se
  // parecen mucho más a la primera transición de un fichero de formato
  // APv (que no lleva línea de F) que a un F genuino.
  if (tokens.size() == 5) {
    bool all_tokens_are_declared_states = true;
    for (const std::string& token : tokens) {
      if (states.count(State(token)) == 0) {
        all_tokens_are_declared_states = false;
        break;
      }
    }
    if (!all_tokens_are_declared_states) {
      throw MissingSectionError(
          "El fichero parece corresponder a un autómata "
          "por vaciado de pila (APv): falta la línea de F. Este simulador solo "
          "reconoce autómatas con finalización por estado final (APf).",
          current_line.line_number);
    }
  }

  std::vector<State> final_states;
  for (const std::string& token : tokens) {
    final_states.emplace_back(token);
  }
  CheckNoDuplicateStates(final_states, "F", current_line.line_number);
  for (const State& final_state : final_states) {
    if (states.count(final_state) == 0) {
      throw InvalidStateError(
          "El estado final '" + final_state.GetName() + "' no pertenece a Q.",
          current_line.line_number);
    }
  }
  return final_states;
}

Transition AutomatonParser::ParseTransitionLine(
    const SourceLine& source_line, const std::set<State>& states,
    const Alphabet& input_alphabet, const Alphabet& stack_alphabet) {
  const std::vector<std::string> tokens = SplitIntoTokens(source_line.content);
  if (tokens.size() != 5) {
    throw InvalidTransitionError(
        "La transición '" + source_line.content +
            "' no es válida: debe tener exactamente 5 campos (origen, entrada, "
            "cima, destino, símbolos a apilar); tiene " +
            std::to_string(tokens.size()) + ".",
        source_line.line_number);
  }

  const std::string& origin_token = tokens[0];
  const std::string& input_token = tokens[1];
  const std::string& stack_top_token = tokens[2];
  const std::string& destination_token = tokens[3];
  const std::string& push_token = tokens[4];

  const State origin_state(origin_token);
  if (states.count(origin_state) == 0) {
    throw InvalidTransitionError(
        "El estado de origen '" + origin_token + "' de la transición '" +
            source_line.content + "' no está declarado en Q.",
        source_line.line_number);
  }
  const State destination_state(destination_token);
  if (states.count(destination_state) == 0) {
    throw InvalidTransitionError(
        "El estado de destino '" + destination_token + "' de la transición '" +
            source_line.content + "' no está declarado en Q.",
        source_line.line_number);
  }

  if (input_token.length() != 1) {
    throw InvalidTransitionError(
        "El símbolo de entrada '" + input_token + "' de la transición '" +
            source_line.content +
            "' no es válido: debe tener un único carácter.",
        source_line.line_number);
  }
  const Symbol input_symbol(input_token[0]);
  if (!input_symbol.IsEpsilon() && !input_alphabet.Contains(input_symbol)) {
    throw InvalidTransitionError(
        "El símbolo de entrada '" + input_token + "' de la transición '" +
            source_line.content + "' no pertenece a Σ ∪ {ε}.",
        source_line.line_number);
  }

  if (stack_top_token.length() != 1) {
    throw InvalidTransitionError(
        "El símbolo de la cima '" + stack_top_token + "' de la transición '" +
            source_line.content +
            "' no es válido: debe tener un único carácter.",
        source_line.line_number);
  }
  if (stack_top_token[0] == kEpsilonCharacter) {
    throw InvalidTransitionError("La transición '" + source_line.content +
                                     "' no es válida: una transición nunca "
                                     "puede consultar ε en la cima de "
                                     "la pila.",
                                 source_line.line_number);
  }
  const Symbol stack_symbol(stack_top_token[0]);
  if (!stack_alphabet.Contains(stack_symbol)) {
    throw InvalidTransitionError(
        "El símbolo de la cima '" + stack_top_token + "' de la transición '" +
            source_line.content + "' no pertenece a Γ.",
        source_line.line_number);
  }

  std::string symbols_to_push;
  if (push_token == std::string(1, kEpsilonCharacter)) {
    symbols_to_push = "";
  } else {
    if (push_token.find(kEpsilonCharacter) != std::string::npos) {
      throw InvalidTransitionError(
          "La secuencia a apilar '" + push_token + "' de la transición '" +
              source_line.content +
              "' no es válida: no puede mezclar '.' con otros símbolos.",
          source_line.line_number);
    }
    for (char pushed_character : push_token) {
      if (!stack_alphabet.Contains(Symbol(pushed_character))) {
        throw InvalidTransitionError("La secuencia a apilar '" + push_token +
                                         "' de la transición '" +
                                         source_line.content +
                                         "' contiene el "
                                         "símbolo '" +
                                         std::string(1, pushed_character) +
                                         "', que no "
                                         "pertenece a Γ.",
                                     source_line.line_number);
      }
    }
    symbols_to_push = push_token;
  }

  return Transition(origin_state, input_symbol, stack_symbol, destination_state,
                    symbols_to_push);
}

PushdownAutomaton AutomatonParser::Parse(std::ostream& warnings_stream) {
  significant_lines_ = ReadSignificantLines();
  current_line_index_ = 0;

  const std::vector<State> declared_states =
      ParseStatesSection("Q", /*allow_empty=*/false);
  const std::set<State> states(declared_states.begin(), declared_states.end());

  const Alphabet input_alphabet = ParseAlphabetSection("Σ");
  const Alphabet stack_alphabet = ParseAlphabetSection("Γ");

  const State initial_state = ParseInitialStateSection(states);
  const Symbol initial_stack_symbol =
      ParseInitialStackSymbolSection(stack_alphabet);
  const std::vector<State> final_states = ParseFinalStatesSection(states);

  TransitionFunction transition_function;
  SourceLine transition_line;
  while (TryGetNextLine(transition_line)) {
    const Transition transition = ParseTransitionLine(
        transition_line, states, input_alphabet, stack_alphabet);
    if (!transition_function.Insert(transition)) {
      warnings_stream << "[Aviso] Transición duplicada, se ignora: "
                      << transition << "\n";
    }
  }

  return PushdownAutomaton(declared_states, input_alphabet, stack_alphabet,
                           initial_state, initial_stack_symbol, final_states,
                           transition_function);
}