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
 * @file turing_machine_parser.h
 * @brief Definición de TuringMachineParser: construye una TuringMachine a
 * partir del fichero de configuración, validando todo lo que debe abortar
 * la carga (mismo patrón que AutomatonParser de P01: lee el fichero entero
 * de una vez con el número de línea real de cada línea, y fija ese número
 * en cualquier ConfigurationError que llegue sin él).
 *
 * Orden del fichero: Q, Σ, Γ, q0, b (blanco), F, número de cintas,
 * transiciones.
 */

#ifndef TURING_MACHINE_PARSER_H_
#define TURING_MACHINE_PARSER_H_

#include <cstddef>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "alphabet.h"
#include "state.h"
#include "symbol.h"
#include "transition.h"
#include "turing_machine.h"

class TuringMachineParser {
 public:
  explicit TuringMachineParser(const std::string& configuration_file_path)
      : configuration_file_path_(configuration_file_path) {}

  /**
   * @param warnings_stream Donde se informa de las transiciones duplicadas ("[Aviso] ...").
   * @throw FileError Si el fichero no se puede abrir, o está vacío.
   * @throw ConfigurationError (o subclase) con el número de línea real fijado.
   */
  TuringMachine Parse(std::ostream& warnings_stream);

 private:
  struct SourceLine {
    int line_number;
    std::string content;
  };

  std::vector<SourceLine> ReadSignificantLines() const;
  static std::string StripComment(const std::string& line);
  static bool IsBlankLine(const std::string& line);
  static std::vector<std::string> SplitIntoTokens(const std::string& line);
  bool TryGetNextLine(SourceLine& next_line);

  std::vector<State> ParseStatesSection(const std::string& section_name, bool allow_empty);
  Alphabet ParseAlphabetSection(const std::string& section_name);
  State ParseInitialStateSection(const std::set<State>& states);

  /** @throw InvalidSymbolError Si no tiene un único carácter, no pertenece a Γ, o pertenece a Σ. */
  Symbol ParseBlankSymbolSection(const Alphabet& input_alphabet, const Alphabet& tape_alphabet);

  std::vector<State> ParseFinalStatesSection(const std::set<State>& states);

  /** @throw MissingSectionError, ConfigurationError si no es un entero positivo. */
  int ParseTapeCountSection();

  /** @throw InvalidTransitionError */
  Transition ParseTransitionLine(const SourceLine& source_line, const std::set<State>& states,
                                 const Alphabet& tape_alphabet, int tape_count);

  /** @throw DuplicatedElementError */
  void CheckNoDuplicateStates(const std::vector<State>& states, const std::string& section_name,
                              int line_number) const;

  std::string configuration_file_path_;
  std::vector<SourceLine> significant_lines_;
  std::size_t current_line_index_ = 0;
};

#endif  // TURING_MACHINE_PARSER_H_
