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
 * @file turing_machine_parser.cc
 * @brief Implementación de la clase TuringMachineParser.
 */

#include "../include/turing_machine_parser.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include "../include/errors.h"

namespace {

/** @brief Los cinco tipos de campo de una transición, en el orden en que
 * aparecen. */
enum class FieldKind {
  kOrigin,
  kReadSymbol,
  kDestination,
  kWriteSymbol,
  kMovement
};

/** @brief Plantilla de campos de una transición con tape_count cintas: origen,
 * N lee, destino, N escribe, N mueve. */
std::vector<FieldKind> BuildFieldLayout(int tape_count) {
  std::vector<FieldKind> layout{FieldKind::kOrigin};
  layout.insert(layout.end(), tape_count, FieldKind::kReadSymbol);
  layout.push_back(FieldKind::kDestination);
  layout.insert(layout.end(), tape_count, FieldKind::kWriteSymbol);
  layout.insert(layout.end(), tape_count, FieldKind::kMovement);
  return layout;
}

std::string FieldName(FieldKind kind, int tape_count) {
  const std::string per_tape = tape_count > 1 ? " de alguna de las cintas" : "";
  switch (kind) {
    case FieldKind::kOrigin:
      return "el estado de origen";
    case FieldKind::kReadSymbol:
      return "el símbolo leído" + per_tape;
    case FieldKind::kDestination:
      return "el estado de destino";
    case FieldKind::kWriteSymbol:
      return "el símbolo a escribir" + per_tape;
    case FieldKind::kMovement:
      return "el movimiento" + per_tape;
  }
  return "";
}

/** @brief ¿El token puede ser un campo de ese tipo? (estado de Q, símbolo de Γ,
 * o movimiento L/R/S) */
bool TokenFitsField(const std::string& token, FieldKind kind,
                    const std::set<State>& states,
                    const Alphabet& tape_alphabet) {
  switch (kind) {
    case FieldKind::kOrigin:
    case FieldKind::kDestination:
      return states.count(State(token)) > 0;
    case FieldKind::kReadSymbol:
    case FieldKind::kWriteSymbol:
      return token.length() == 1 && tape_alphabet.Contains(Symbol(token[0]));
    case FieldKind::kMovement:
      return token == "L" || token == "R" || token == "S";
  }
  return false;
}

/**
 * @brief Si falta o sobra exactamente un campo, intenta deducir cuál: busca en
 * qué posición de la plantilla encajan todos los demás campos. Es solo una
 * pista para el mensaje de error (nunca cambia si la transición se acepta);
 * devuelve "" si no se puede deducir.
 */
std::string InferFieldCountHint(const std::vector<std::string>& tokens,
                                const std::vector<FieldKind>& layout,
                                const std::set<State>& states,
                                const Alphabet& tape_alphabet, int tape_count) {
  if (tokens.size() + 1 == layout.size()) {
    std::vector<FieldKind> candidates;
    for (std::size_t missing = 0; missing < layout.size(); ++missing) {
      bool fits = true;
      for (std::size_t i = 0; i < tokens.size() && fits; ++i) {
        fits = TokenFitsField(tokens[i], layout[i < missing ? i : i + 1],
                              states, tape_alphabet);
      }
      if (fits && std::find(candidates.begin(), candidates.end(),
                            layout[missing]) == candidates.end()) {
        candidates.push_back(layout[missing]);
      }
    }
    if (candidates.empty()) return "";
    std::string names;
    for (std::size_t i = 0; i < candidates.size(); ++i) {
      names += (i == 0 ? "" : " o ") + FieldName(candidates[i], tape_count);
    }
    return (candidates.size() == 1 ? " Parece que falta " : " Podría faltar ") +
           names + ".";
  }
  if (tokens.size() == layout.size() + 1) {
    std::vector<std::string> candidates;
    for (std::size_t extra = 0; extra < tokens.size(); ++extra) {
      bool fits = true;
      std::size_t layout_index = 0;
      for (std::size_t i = 0; i < tokens.size() && fits; ++i) {
        if (i == extra) continue;
        fits = TokenFitsField(tokens[i], layout[layout_index++], states,
                              tape_alphabet);
      }
      if (fits && std::find(candidates.begin(), candidates.end(),
                            tokens[extra]) == candidates.end()) {
        candidates.push_back(tokens[extra]);
      }
    }
    if (candidates.empty()) return "";
    if (candidates.size() == 1)
      return " Parece que sobra el campo '" + candidates[0] + "'.";
    std::string list;
    for (std::size_t i = 0; i < candidates.size(); ++i)
      list += (i == 0 ? "'" : ", '") + candidates[i] + "'";
    return " Podría sobrar uno de estos campos: " + list + ".";
  }
  return "";
}

}  // namespace

std::string TuringMachineParser::StripComment(const std::string& line) {
  const std::size_t comment_position = line.find('#');
  return comment_position == std::string::npos
             ? line
             : line.substr(0, comment_position);
}

bool TuringMachineParser::IsBlankLine(const std::string& line) {
  return line.find_first_not_of(" \t\r\n") == std::string::npos;
}

std::vector<std::string> TuringMachineParser::SplitIntoTokens(
    const std::string& line) {
  std::vector<std::string> tokens;
  std::istringstream token_stream(line);
  std::string token;
  while (token_stream >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

std::vector<TuringMachineParser::SourceLine>
TuringMachineParser::ReadSignificantLines() const {
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

bool TuringMachineParser::TryGetNextLine(SourceLine& next_line) {
  if (current_line_index_ >= significant_lines_.size()) {
    return false;
  }
  next_line = significant_lines_[current_line_index_];
  ++current_line_index_;
  return true;
}

void TuringMachineParser::CheckNoDuplicateStates(
    const std::vector<State>& states, const std::string& section_name,
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

std::vector<State> TuringMachineParser::ParseStatesSection(
    const std::string& section_name, bool allow_empty) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: falta la línea de " +
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

Alphabet TuringMachineParser::ParseAlphabetSection(
    const std::string& section_name) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: falta la línea de " +
        section_name + ".");
  }
  try {
    return Alphabet(current_line.content);
  } catch (ConfigurationError& error) {
    error.SetLineNumber(current_line.line_number);
    throw;
  }
}

State TuringMachineParser::ParseInitialStateSection(
    const std::set<State>& states) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: falta la línea de q0.");
  }
  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);
  if (tokens.size() != 1) {
    throw MissingSectionError(
        "El estado inicial (q0) debe ser exactamente un estado; se han "
        "encontrado " +
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

Symbol TuringMachineParser::ParseBlankSymbolSection(
    const Alphabet& input_alphabet, const Alphabet& tape_alphabet) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: falta la línea del "
        "símbolo blanco.");
  }
  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);
  if (tokens.size() != 1) {
    throw MissingSectionError(
        "El símbolo blanco debe ser exactamente un símbolo; se han "
        "encontrado " +
            std::to_string(tokens.size()) + ".",
        current_line.line_number);
  }
  if (tokens.front().length() != 1) {
    throw InvalidSymbolError(
        "El símbolo blanco '" + tokens.front() +
            "' no es válido: debe tener un único carácter.",
        current_line.line_number);
  }
  const Symbol blank_symbol(tokens.front()[0]);
  if (!tape_alphabet.Contains(blank_symbol)) {
    throw InvalidSymbolError(
        "El símbolo blanco '" + tokens.front() + "' no pertenece a Γ.",
        current_line.line_number);
  }
  if (input_alphabet.Contains(blank_symbol)) {
    throw InvalidSymbolError(
        "El símbolo blanco '" + tokens.front() + "' no puede pertenecer a Σ.",
        current_line.line_number);
  }
  return blank_symbol;
}

std::vector<State> TuringMachineParser::ParseFinalStatesSection(
    const std::set<State>& states) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: falta la línea de F.");
  }
  std::vector<State> final_states;
  for (const std::string& token : SplitIntoTokens(current_line.content)) {
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

int TuringMachineParser::ParseTapeCountSection() {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError(
        "El fichero de configuración está incompleto: falta la línea del "
        "número de cintas.");
  }
  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);
  if (tokens.size() != 1) {
    throw MissingSectionError(
        "El número de cintas debe ser un único valor; se han encontrado " +
            std::to_string(tokens.size()) + ".",
        current_line.line_number);
  }
  const std::string& token = tokens.front();
  const bool is_all_digits =
      !token.empty() &&
      token.find_first_not_of("0123456789") == std::string::npos;
  if (!is_all_digits) {
    throw ConfigurationError("El número de cintas '" + token +
                                 "' no es un entero positivo "
                                 "válido.",
                             current_line.line_number);
  }
  int tape_count = 0;
  try {
    tape_count = std::stoi(token);
  } catch (const std::out_of_range&) {
    throw ConfigurationError(
        "El número de cintas '" + token + "' es demasiado grande.",
        current_line.line_number);
  }
  if (tape_count < 1) {
    throw ConfigurationError("El número de cintas debe ser mayor que 0.",
                             current_line.line_number);
  }
  return tape_count;
}

Transition TuringMachineParser::ParseTransitionLine(
    const SourceLine& source_line, const std::set<State>& states,
    const Alphabet& tape_alphabet, int tape_count) {
  const std::vector<std::string> tokens = SplitIntoTokens(source_line.content);
  const std::size_t expected_token_count =
      static_cast<std::size_t>(2 + 3 * tape_count);
  if (tokens.size() != expected_token_count) {
    throw InvalidTransitionError(
        "La transición '" + source_line.content +
            "' no es válida: se "
            "esperaban " +
            std::to_string(expected_token_count) + " campos (origen, " +
            std::to_string(tape_count) + " símbolo(s) leído(s), destino, " +
            std::to_string(tape_count) + " símbolo(s) a escribir, " +
            std::to_string(tape_count) + " movimiento(s)); tiene " +
            std::to_string(tokens.size()) + "." +
            InferFieldCountHint(tokens, BuildFieldLayout(tape_count), states,
                                tape_alphabet, tape_count),
        source_line.line_number);
  }

  std::size_t index = 0;
  const std::string& origin_token = tokens[index++];
  const State origin_state(origin_token);
  if (states.count(origin_state) == 0) {
    throw InvalidTransitionError(
        "El estado de origen '" + origin_token + "' de la transición '" +
            source_line.content + "' no está declarado en Q.",
        source_line.line_number);
  }

  std::vector<Symbol> read_symbols;
  for (int tape_index = 0; tape_index < tape_count; ++tape_index) {
    const std::string& token = tokens[index++];
    if (token.length() != 1) {
      throw InvalidTransitionError(
          "El símbolo leído '" + token + "' de la transición '" +
              source_line.content +
              "' no es válido: debe tener un único carácter.",
          source_line.line_number);
    }
    const Symbol symbol(token[0]);
    if (!tape_alphabet.Contains(symbol)) {
      throw InvalidTransitionError(
          "El símbolo leído '" + token + "' de la transición '" +
              source_line.content + "' no pertenece a Γ.",
          source_line.line_number);
    }
    read_symbols.push_back(symbol);
  }

  const std::string& destination_token = tokens[index++];
  const State destination_state(destination_token);
  if (states.count(destination_state) == 0) {
    throw InvalidTransitionError(
        "El estado de destino '" + destination_token + "' de la transición '" +
            source_line.content + "' no está declarado en Q.",
        source_line.line_number);
  }

  std::vector<Symbol> write_symbols;
  for (int tape_index = 0; tape_index < tape_count; ++tape_index) {
    const std::string& token = tokens[index++];
    if (token.length() != 1) {
      throw InvalidTransitionError(
          "El símbolo a escribir '" + token + "' de la transición '" +
              source_line.content +
              "' no es válido: debe tener un único carácter.",
          source_line.line_number);
    }
    const Symbol symbol(token[0]);
    if (!tape_alphabet.Contains(symbol)) {
      throw InvalidTransitionError(
          "El símbolo a escribir '" + token + "' de la transición '" +
              source_line.content + "' no pertenece a Γ.",
          source_line.line_number);
    }
    write_symbols.push_back(symbol);
  }

  std::vector<Movement> movements;
  for (int tape_index = 0; tape_index < tape_count; ++tape_index) {
    const std::string& token = tokens[index++];
    if (token == "L") {
      movements.push_back(Movement::kLeft);
    } else if (token == "R") {
      movements.push_back(Movement::kRight);
    } else if (token == "S") {
      movements.push_back(Movement::kStay);
    } else {
      throw InvalidTransitionError(
          "El movimiento '" + token + "' de la transición '" +
              source_line.content + "' no es válido: debe ser 'L', 'R' o 'S'.",
          source_line.line_number);
    }
  }

  std::vector<TapeAction> tape_actions;
  for (int tape_index = 0; tape_index < tape_count; ++tape_index) {
    tape_actions.push_back(TapeAction{read_symbols[tape_index],
                                      write_symbols[tape_index],
                                      movements[tape_index]});
  }

  return Transition(origin_state, destination_state, tape_actions);
}

TuringMachine TuringMachineParser::Parse(std::ostream& warnings_stream) {
  significant_lines_ = ReadSignificantLines();
  current_line_index_ = 0;

  const std::vector<State> declared_states =
      ParseStatesSection("Q", /*allow_empty=*/false);
  const std::set<State> states(declared_states.begin(), declared_states.end());

  const Alphabet input_alphabet = ParseAlphabetSection("Σ");
  const Alphabet tape_alphabet = ParseAlphabetSection("Γ");

  const State initial_state = ParseInitialStateSection(states);
  const Symbol blank_symbol =
      ParseBlankSymbolSection(input_alphabet, tape_alphabet);
  const std::vector<State> final_states = ParseFinalStatesSection(states);
  const int tape_count = ParseTapeCountSection();

  TransitionFunction transition_function;
  SourceLine transition_line;
  while (TryGetNextLine(transition_line)) {
    const Transition transition =
        ParseTransitionLine(transition_line, states, tape_alphabet, tape_count);
    try {
      if (!transition_function.Insert(transition)) {
        warnings_stream << "[Aviso] Transición duplicada, se ignora: "
                        << transition << "\n";
      }
    } catch (ConfigurationError& error) {
      error.SetLineNumber(transition_line.line_number);
      throw;
    }
  }

  return TuringMachine(declared_states, input_alphabet, tape_alphabet,
                       initial_state, blank_symbol, final_states, tape_count,
                       transition_function);
}