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
#include <array>
#include <fstream>
#include <functional>
#include <sstream>

#include "../include/errors.h"

namespace {

/** @brief Los cinco tipos de campo de una transición, en el orden en que aparecen. */
enum class FieldKind { kOrigin, kReadSymbol, kDestination, kWriteSymbol, kMovement };

/** @brief Plantilla de campos de una transición con tape_count cintas: origen, N lee, destino, N escribe, N mueve. */
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
    case FieldKind::kOrigin: return "el estado de origen";
    case FieldKind::kReadSymbol: return "el símbolo leído" + per_tape;
    case FieldKind::kDestination: return "el estado de destino";
    case FieldKind::kWriteSymbol: return "el símbolo a escribir" + per_tape;
    case FieldKind::kMovement: return "el movimiento" + per_tape;
  }
  return "";
}

/** @brief ¿El token puede ser un campo de ese tipo? (estado de Q, símbolo de Γ, o movimiento L/R/S) */
bool TokenFitsField(const std::string& token, FieldKind kind, const std::set<State>& states,
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

/** @brief "1 estado" / "2 estados" seguido de la lista entre comillas: "2 estados ('q0', 'q1')". */
std::string CountedList(const std::vector<std::string>& items, const std::string& singular,
                        const std::string& plural) {
  std::string text = std::to_string(items.size()) + " " + (items.size() == 1 ? singular : plural);
  for (std::size_t i = 0; i < items.size(); ++i) text += (i == 0 ? " ('" : ", '") + items[i] + "'";
  return items.empty() ? text : text + ")";
}

/**
 * @brief Los campos encontrados en la línea, agrupados por lo que son (estado de
 * Q, símbolo de Γ, movimiento L/R/S, o no reconocido), para compararlos con lo
 * esperado. No deduce nada: describe lo que hay.
 */
std::string DescribeFoundFields(const std::vector<std::string>& tokens, const std::set<State>& states,
                                const Alphabet& tape_alphabet) {
  std::vector<std::string> found_states, found_symbols, found_movements, unrecognized;
  for (const std::string& token : tokens) {
    if (states.count(State(token)) > 0) {
      found_states.push_back(token);
    } else if (token == "L" || token == "R" || token == "S") {
      found_movements.push_back(token);
    } else if (token.length() == 1 && tape_alphabet.Contains(Symbol(token[0]))) {
      found_symbols.push_back(token);
    } else {
      unrecognized.push_back(token);
    }
  }
  std::string text = CountedList(found_states, "estado", "estados") + "; " +
                     CountedList(found_symbols, "símbolo", "símbolos") + "; " +
                     CountedList(found_movements, "movimiento", "movimientos");
  if (!unrecognized.empty()) text += "; " + CountedList(unrecognized, "no reconocido", "no reconocidos");
  return text;
}

constexpr int kFieldKindCount = 5;
constexpr std::size_t kMaxHintedFields = 3;  // Con más campos de diferencia no se intenta deducir nada.
constexpr std::size_t kMaxHintAlternatives = 3;  // Con más posibilidades, una lista no ayuda: sin pista.
using KindCounts = std::array<int, kFieldKindCount>;
using AlignmentSet = std::set<KindCounts, std::greater<KindCounts>>;  // Orden: el de FieldKind.

/**
 * @brief Recorre todas las formas de repartir los tokens en la plantilla dejando
 * algunos campos sin rellenar, y recoge, por cada reparto válido (todos los
 * tokens encajan en el campo que les toca), cuántos campos faltan de cada tipo.
 */
void CollectMissingAlignments(const std::vector<std::string>& tokens,
                              const std::vector<FieldKind>& layout, const std::set<State>& states,
                              const Alphabet& tape_alphabet, std::size_t token_index,
                              std::size_t layout_index, KindCounts& missing, AlignmentSet& alignments) {
  const std::size_t tokens_left = tokens.size() - token_index;
  const std::size_t layout_left = layout.size() - layout_index;
  if (tokens_left > layout_left) return;
  if (tokens_left == 0) {
    KindCounts total = missing;
    for (std::size_t j = layout_index; j < layout.size(); ++j) ++total[static_cast<int>(layout[j])];
    alignments.insert(total);
    return;
  }
  if (TokenFitsField(tokens[token_index], layout[layout_index], states, tape_alphabet)) {
    CollectMissingAlignments(tokens, layout, states, tape_alphabet, token_index + 1, layout_index + 1,
                             missing, alignments);
  }
  if (tokens_left < layout_left) {
    ++missing[static_cast<int>(layout[layout_index])];
    CollectMissingAlignments(tokens, layout, states, tape_alphabet, token_index, layout_index + 1,
                             missing, alignments);
    --missing[static_cast<int>(layout[layout_index])];
  }
}

/** @brief P. ej. "el símbolo leído y el símbolo a escribir (de alguna de las cintas)". */
std::string DescribeMissingFields(const KindCounts& counts, int tape_count) {
  std::vector<std::string> parts;
  bool has_per_tape_field = false;
  for (int kind = 0; kind < kFieldKindCount; ++kind) {
    const int n = counts[kind];
    if (n == 0) continue;
    const bool single_tape = tape_count == 1;
    switch (static_cast<FieldKind>(kind)) {
      case FieldKind::kOrigin: parts.push_back("el estado de origen"); break;
      case FieldKind::kDestination: parts.push_back("el estado de destino"); break;
      case FieldKind::kReadSymbol:
        has_per_tape_field = true;
        parts.push_back(n > 1 ? std::to_string(n) + " símbolos leídos" : (single_tape ? "el" : "un") + std::string(" símbolo leído"));
        break;
      case FieldKind::kWriteSymbol:
        has_per_tape_field = true;
        parts.push_back(n > 1 ? std::to_string(n) + " símbolos a escribir" : (single_tape ? "el" : "un") + std::string(" símbolo a escribir"));
        break;
      case FieldKind::kMovement:
        has_per_tape_field = true;
        parts.push_back(n > 1 ? std::to_string(n) + " movimientos" : (single_tape ? "el" : "un") + std::string(" movimiento"));
        break;
    }
  }
  std::string text;
  for (std::size_t i = 0; i < parts.size(); ++i) {
    text += (i == 0 ? "" : (i + 1 == parts.size() ? " y " : ", ")) + parts[i];
  }
  return text + (has_per_tape_field && tape_count > 1 ? " (de alguna de las cintas)" : "");
}

/** @brief Pista para cuando faltan campos (1 a kMaxHintedFields): cuáles, si se puede deducir. */
std::string HintForMissingFields(const std::vector<std::string>& tokens,
                                 const std::vector<FieldKind>& layout, const std::set<State>& states,
                                 const Alphabet& tape_alphabet, int tape_count) {
  const std::size_t missing_count = layout.size() - tokens.size();
  AlignmentSet alignments;
  KindCounts missing{};
  CollectMissingAlignments(tokens, layout, states, tape_alphabet, 0, 0, missing, alignments);
  if (alignments.empty() || alignments.size() > kMaxHintAlternatives) return "";

  std::string alternatives;
  for (const KindCounts& counts : alignments) {
    std::string description;
    if (missing_count == 1) {  // Un solo campo: "el símbolo leído de alguna de las cintas".
      for (int kind = 0; kind < kFieldKindCount; ++kind) {
        if (counts[kind] > 0) description = FieldName(static_cast<FieldKind>(kind), tape_count);
      }
    } else {
      description = DescribeMissingFields(counts, tape_count);
    }
    alternatives += (alternatives.empty() ? "" : " o ") + description;
  }
  if (alignments.size() > 1) return " Podría faltar " + alternatives + ".";
  if (missing_count == 1) return " Parece que falta " + alternatives + ".";
  return " Parece que faltan " + std::to_string(missing_count) + " campos: " + alternatives + ".";
}

/** @brief Recorre los repartos en los que algunos tokens sobran; recoge los grupos de tokens sobrantes. */
void CollectExtraAlignments(const std::vector<std::string>& tokens, const std::vector<FieldKind>& layout,
                            const std::set<State>& states, const Alphabet& tape_alphabet,
                            std::size_t token_index, std::size_t layout_index,
                            std::vector<std::string>& extras, std::set<std::vector<std::string>>& groups) {
  const std::size_t tokens_left = tokens.size() - token_index;
  const std::size_t layout_left = layout.size() - layout_index;
  if (layout_left > tokens_left) return;
  if (layout_left == 0) {
    std::vector<std::string> group = extras;
    group.insert(group.end(), tokens.begin() + token_index, tokens.end());
    groups.insert(group);
    return;
  }
  if (TokenFitsField(tokens[token_index], layout[layout_index], states, tape_alphabet)) {
    CollectExtraAlignments(tokens, layout, states, tape_alphabet, token_index + 1, layout_index + 1, extras, groups);
  }
  if (tokens_left > layout_left) {
    extras.push_back(tokens[token_index]);
    CollectExtraAlignments(tokens, layout, states, tape_alphabet, token_index + 1, layout_index, extras, groups);
    extras.pop_back();
  }
}

/** @brief Pista para cuando sobran campos (1 a kMaxHintedFields): cuáles, si se puede deducir. */
std::string HintForExtraFields(const std::vector<std::string>& tokens,
                               const std::vector<FieldKind>& layout, const std::set<State>& states,
                               const Alphabet& tape_alphabet) {
  std::set<std::vector<std::string>> groups;
  std::vector<std::string> extras;
  CollectExtraAlignments(tokens, layout, states, tape_alphabet, 0, 0, extras, groups);
  if (groups.empty() || groups.size() > kMaxHintAlternatives) return "";

  auto quoted_list = [](const std::vector<std::string>& group) {
    std::string list;
    for (std::size_t i = 0; i < group.size(); ++i) list += (i == 0 ? "'" : ", '") + group[i] + "'";
    return list;
  };
  const bool single_field = tokens.size() == layout.size() + 1;
  if (groups.size() == 1) {
    return (single_field ? " Parece que sobra el campo " : " Parece que sobran los campos ") + quoted_list(*groups.begin()) + ".";
  }
  std::string options;
  for (const auto& group : groups) {
    options += (options.empty() ? "" : (single_field ? ", " : "; ")) + (single_field ? quoted_list(group) : "(" + quoted_list(group) + ")");
  }
  return (single_field ? " Podría sobrar uno de estos campos: " : " Podría sobrar uno de estos grupos de campos: ") + options + ".";
}

/**
 * @brief Pista para el mensaje de "número de campos incorrecto": intenta
 * deducir qué campos faltan o sobran, buscando en qué posición de la plantilla
 * encajan los que sí hay. Es solo una pista (nunca cambia si la transición se
 * acepta); devuelve "" si la diferencia es mayor que kMaxHintedFields o no
 * se puede deducir.
 */
std::string InferFieldCountHint(const std::vector<std::string>& tokens,
                                const std::vector<FieldKind>& layout, const std::set<State>& states,
                                const Alphabet& tape_alphabet, int tape_count) {
  const std::size_t difference = tokens.size() > layout.size() ? tokens.size() - layout.size()
                                                               : layout.size() - tokens.size();
  if (difference == 0 || difference > kMaxHintedFields) return "";
  return tokens.size() < layout.size()
             ? HintForMissingFields(tokens, layout, states, tape_alphabet, tape_count)
             : HintForExtraFields(tokens, layout, states, tape_alphabet);
}

}  // namespace

std::string TuringMachineParser::StripComment(const std::string& line) {
  const std::size_t comment_position = line.find('#');
  return comment_position == std::string::npos ? line : line.substr(0, comment_position);
}

bool TuringMachineParser::IsBlankLine(const std::string& line) {
  return line.find_first_not_of(" \t\r\n") == std::string::npos;
}

std::vector<std::string> TuringMachineParser::SplitIntoTokens(const std::string& line) {
  std::vector<std::string> tokens;
  std::istringstream token_stream(line);
  std::string token;
  while (token_stream >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

std::vector<TuringMachineParser::SourceLine> TuringMachineParser::ReadSignificantLines() const {
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
    throw FileError("El fichero de configuración '" + configuration_file_path_ + "' está vacío.");
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

void TuringMachineParser::CheckNoDuplicateStates(const std::vector<State>& states,
                                                 const std::string& section_name,
                                                 int line_number) const {
  std::set<State> seen_states;
  for (const State& state : states) {
    if (!seen_states.insert(state).second) {
      throw DuplicatedElementError("El estado '" + state.GetName() + "' aparece repetido en " +
          section_name + ".", line_number);
    }
  }
}

std::vector<State> TuringMachineParser::ParseStatesSection(const std::string& section_name,
                                                           bool allow_empty) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError("El fichero de configuración está incompleto: falta la línea de " +
        section_name + ".");
  }

  std::vector<State> states;
  for (const std::string& token : SplitIntoTokens(current_line.content)) {
    states.emplace_back(token);
  }
  if (!allow_empty && states.empty()) {
    throw MissingSectionError(section_name + " no puede estar vacío.", current_line.line_number);
  }
  CheckNoDuplicateStates(states, section_name, current_line.line_number);
  return states;
}

Alphabet TuringMachineParser::ParseAlphabetSection(const std::string& section_name) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError("El fichero de configuración está incompleto: falta la línea de " +
        section_name + ".");
  }
  try {
    return Alphabet(current_line.content);
  } catch (ConfigurationError& error) {
    error.SetLineNumber(current_line.line_number);
    throw;
  }
}

State TuringMachineParser::ParseInitialStateSection(const std::set<State>& states) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError("El fichero de configuración está incompleto: falta la línea de q0.");
  }
  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);
  if (tokens.size() != 1) {
    throw MissingSectionError("El estado inicial (q0) debe ser exactamente un estado; se han "
        "encontrado " + std::to_string(tokens.size()) + ".", current_line.line_number);
  }
  const State initial_state(tokens.front());
  if (states.count(initial_state) == 0) {
    throw InvalidStateError("El estado inicial '" + tokens.front() + "' (q0) no pertenece a Q.",
        current_line.line_number);
  }
  return initial_state;
}

Symbol TuringMachineParser::ParseBlankSymbolSection(const Alphabet& input_alphabet,
                                                    const Alphabet& tape_alphabet) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError("El fichero de configuración está incompleto: falta la línea del "
        "símbolo blanco.");
  }
  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);
  if (tokens.size() != 1) {
    throw MissingSectionError("El símbolo blanco debe ser exactamente un símbolo; se han "
        "encontrado " + std::to_string(tokens.size()) + ".", current_line.line_number);
  }
  if (tokens.front().length() != 1) {
    throw InvalidSymbolError("El símbolo blanco '" + tokens.front() +
        "' no es válido: debe tener un único carácter.", current_line.line_number);
  }
  const Symbol blank_symbol(tokens.front()[0]);
  if (!tape_alphabet.Contains(blank_symbol)) {
    throw InvalidSymbolError("El símbolo blanco '" + tokens.front() + "' no pertenece a Γ.",
        current_line.line_number);
  }
  if (input_alphabet.Contains(blank_symbol)) {
    throw InvalidSymbolError("El símbolo blanco '" + tokens.front() +
        "' no puede pertenecer a Σ.", current_line.line_number);
  }
  return blank_symbol;
}

std::vector<State> TuringMachineParser::ParseFinalStatesSection(const std::set<State>& states) {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError("El fichero de configuración está incompleto: falta la línea de F.");
  }
  std::vector<State> final_states;
  for (const std::string& token : SplitIntoTokens(current_line.content)) {
    final_states.emplace_back(token);
  }
  CheckNoDuplicateStates(final_states, "F", current_line.line_number);
  for (const State& final_state : final_states) {
    if (states.count(final_state) == 0) {
      throw InvalidStateError("El estado final '" + final_state.GetName() + "' no pertenece a Q.",
          current_line.line_number);
    }
  }
  return final_states;
}

int TuringMachineParser::ParseTapeCountSection() {
  SourceLine current_line;
  if (!TryGetNextLine(current_line)) {
    throw MissingSectionError("El fichero de configuración está incompleto: falta la línea del "
        "número de cintas.");
  }
  const std::vector<std::string> tokens = SplitIntoTokens(current_line.content);
  if (tokens.size() != 1) {
    throw MissingSectionError("El número de cintas debe ser un único valor; se han encontrado " +
        std::to_string(tokens.size()) + ".", current_line.line_number);
  }
  const std::string& token = tokens.front();
  const bool is_all_digits = !token.empty() &&
      token.find_first_not_of("0123456789") == std::string::npos;
  if (!is_all_digits) {
    throw ConfigurationError("El número de cintas '" + token + "' no es un entero positivo "
        "válido.", current_line.line_number);
  }
  int tape_count = 0;
  try {
    tape_count = std::stoi(token);
  } catch (const std::out_of_range&) {
    throw ConfigurationError("El número de cintas '" + token + "' es demasiado grande.",
        current_line.line_number);
  }
  if (tape_count < 1) {
    throw ConfigurationError("El número de cintas debe ser mayor que 0.", current_line.line_number);
  }
  return tape_count;
}

Transition TuringMachineParser::ParseTransitionLine(const SourceLine& source_line,
                                                    const std::set<State>& states,
                                                    const Alphabet& tape_alphabet,
                                                    int tape_count) {
  const std::vector<std::string> tokens = SplitIntoTokens(source_line.content);
  const std::size_t expected_token_count = static_cast<std::size_t>(2 + 3 * tape_count);
  if (tokens.size() != expected_token_count) {
    throw InvalidTransitionError("La transición '" + source_line.content + "' no es válida: se "
        "esperaban " + std::to_string(expected_token_count) + " campos (origen, " +
        std::to_string(tape_count) + " símbolo(s) leído(s), destino, " +
        std::to_string(tape_count) + " símbolo(s) a escribir, " + std::to_string(tape_count) +
        " movimiento(s)); se encontraron " + std::to_string(tokens.size()) + ": " +
        DescribeFoundFields(tokens, states, tape_alphabet) + "." +
        InferFieldCountHint(tokens, BuildFieldLayout(tape_count), states, tape_alphabet, tape_count),
        source_line.line_number);
  }

  std::size_t index = 0;
  const std::string& origin_token = tokens[index++];
  const State origin_state(origin_token);
  if (states.count(origin_state) == 0) {
    throw InvalidTransitionError("El estado de origen '" + origin_token + "' de la transición '" +
        source_line.content + "' no está declarado en Q.", source_line.line_number);
  }

  std::vector<Symbol> read_symbols;
  for (int tape_index = 0; tape_index < tape_count; ++tape_index) {
    const std::string& token = tokens[index++];
    if (token.length() != 1) {
      throw InvalidTransitionError("El símbolo leído '" + token + "' de la transición '" +
          source_line.content + "' no es válido: debe tener un único carácter.",
          source_line.line_number);
    }
    const Symbol symbol(token[0]);
    if (!tape_alphabet.Contains(symbol)) {
      throw InvalidTransitionError("El símbolo leído '" + token + "' de la transición '" +
          source_line.content + "' no pertenece a Γ.", source_line.line_number);
    }
    read_symbols.push_back(symbol);
  }

  const std::string& destination_token = tokens[index++];
  const State destination_state(destination_token);
  if (states.count(destination_state) == 0) {
    throw InvalidTransitionError("El estado de destino '" + destination_token +
        "' de la transición '" + source_line.content + "' no está declarado en Q.",
        source_line.line_number);
  }

  std::vector<Symbol> write_symbols;
  for (int tape_index = 0; tape_index < tape_count; ++tape_index) {
    const std::string& token = tokens[index++];
    if (token.length() != 1) {
      throw InvalidTransitionError("El símbolo a escribir '" + token + "' de la transición '" +
          source_line.content + "' no es válido: debe tener un único carácter.",
          source_line.line_number);
    }
    const Symbol symbol(token[0]);
    if (!tape_alphabet.Contains(symbol)) {
      throw InvalidTransitionError("El símbolo a escribir '" + token + "' de la transición '" +
          source_line.content + "' no pertenece a Γ.", source_line.line_number);
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
      throw InvalidTransitionError("El movimiento '" + token + "' de la transición '" +
          source_line.content + "' no es válido: debe ser 'L', 'R' o 'S'.", source_line.line_number);
    }
  }

  std::vector<TapeAction> tape_actions;
  for (int tape_index = 0; tape_index < tape_count; ++tape_index) {
    tape_actions.push_back(TapeAction{read_symbols[tape_index], write_symbols[tape_index],
                                      movements[tape_index]});
  }

  return Transition(origin_state, destination_state, tape_actions);
}

TuringMachine TuringMachineParser::Parse(std::ostream& warnings_stream) {
  significant_lines_ = ReadSignificantLines();
  current_line_index_ = 0;

  const std::vector<State> declared_states = ParseStatesSection("Q", /*allow_empty=*/false);
  const std::set<State> states(declared_states.begin(), declared_states.end());

  const Alphabet input_alphabet = ParseAlphabetSection("Σ");
  const Alphabet tape_alphabet = ParseAlphabetSection("Γ");

  const State initial_state = ParseInitialStateSection(states);
  const Symbol blank_symbol = ParseBlankSymbolSection(input_alphabet, tape_alphabet);
  const std::vector<State> final_states = ParseFinalStatesSection(states);
  const int tape_count = ParseTapeCountSection();

  TransitionFunction transition_function;
  SourceLine transition_line;
  while (TryGetNextLine(transition_line)) {
    const Transition transition =
        ParseTransitionLine(transition_line, states, tape_alphabet, tape_count);
    try {
      if (!transition_function.Insert(transition)) {
        warnings_stream << "[Aviso] Transición duplicada, se ignora: " << transition << "\n";
      }
    } catch (ConfigurationError& error) {
      error.SetLineNumber(transition_line.line_number);
      throw;
    }
  }

  return TuringMachine(declared_states, input_alphabet, tape_alphabet, initial_state,
                       blank_symbol, final_states, tape_count, transition_function);
}