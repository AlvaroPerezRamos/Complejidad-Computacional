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
 * @file automaton_parser.h
 * @brief Definición de AutomatonParser: construye un PushdownAutomaton a
 * partir del fichero de configuración, comprobando todo lo que la
 * sección 6 del enunciado trata como error (ver AutomataPila.md para el
 * reparto de responsabilidades con AutomatonValidator).
 */

#ifndef AUTOMATON_PARSER_H_
#define AUTOMATON_PARSER_H_

#include <cstddef>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "alphabet.h"
#include "pushdown_automaton.h"
#include "state.h"
#include "symbol.h"
#include "transition.h"

/**
 * @class AutomatonParser
 * @brief Construye un PushdownAutomaton a partir de un fichero de
 * configuración, comprobando todas las restricciones que, según el
 * enunciado, deben abortar la carga si se incumplen.
 */
class AutomatonParser {
 public:
  /**
   * @brief Construye el parser para un fichero concreto. No abre ni lee
   * el fichero todavía: eso ocurre en Parse().
   * @param configuration_file_path Ruta del fichero de configuración.
   */
  explicit AutomatonParser(const std::string& configuration_file_path)
      : configuration_file_path_(configuration_file_path) {}

  /** @brief Destructor por defecto. */
  ~AutomatonParser() = default;

  /**
   * @brief Lee el fichero de configuración completo y construye el
   * autómata.
   * @param warnings_stream Flujo donde se informa de las transiciones
   * duplicadas ("[Aviso] ..."): es el único aviso de la sección 6 que se
   * detecta aquí, línea a línea, sin esperar a tener el autómata
   * completo (los demás avisos son cosa de AutomatonValidator).
   * @return El autómata construido.
   * @throw FileError Si el fichero no se puede abrir, o está vacío.
   * @throw ConfigurationError (o alguna subclase) Si el fichero incumple
   * cualquier restricción de la sección 6 del enunciado. Siempre lleva
   * fijado el número de línea real en el que se detectó.
   */
  PushdownAutomaton Parse(std::ostream& warnings_stream);

 private:
  /** @brief Una línea significativa: su número de línea real y su contenido, ya
   * sin comentario. */
  struct SourceLine {
    int line_number;
    std::string content;
  };

  /**
   * @brief Abre el fichero y devuelve sus líneas significativas (sin
   * comentarios ni líneas en blanco), cada una con su número de línea
   * real.
   * @throw FileError Si el fichero no se puede abrir, o no tiene ninguna
   * línea significativa.
   */
  std::vector<SourceLine> ReadSignificantLines() const;

  /** @brief Elimina el comentario de una línea: todo lo que sigue al primer
   * '#'. */
  static std::string StripComment(const std::string& line);

  /** @brief Indica si una línea, sin su comentario, no tiene ningún carácter
   * que no sea espacio en blanco. */
  static bool IsBlankLine(const std::string& line);

  /** @brief Divide una línea en los tokens separados por espacios en blanco que
   * contiene. */
  static std::vector<std::string> SplitIntoTokens(const std::string& line);

  /**
   * @brief Devuelve la siguiente línea significativa todavía no
   * consumida.
   * @param next_line Donde se deja la línea, si se encuentra.
   * @return true si quedaba alguna línea; false si ya se llegó al final.
   */
  bool TryGetNextLine(SourceLine& next_line);

  /**
   * @brief Lee la línea de un conjunto de estados (Q).
   * @param section_name Nombre de la sección, para los mensajes de error.
   * @param allow_empty Si el conjunto puede estar vacío.
   * @throw MissingSectionError Si falta la línea, o si está vacía y
   * allow_empty es false.
   * @throw DuplicatedElementError Si algún estado aparece repetido.
   */
  std::vector<State> ParseStatesSection(const std::string& section_name,
                                        bool allow_empty);

  /**
   * @brief Lee la línea de un alfabeto (Σ o Γ), fijando el número de
   * línea real en cualquier ConfigurationError que Alphabet lance sin él.
   * @param section_name Nombre de la sección, para los mensajes de error.
   */
  Alphabet ParseAlphabetSection(const std::string& section_name);

  /**
   * @brief Lee la línea de q0 y comprueba que pertenece a Q.
   * @param states Conjunto de estados (Q), ya leído.
   * @throw MissingSectionError Si falta la línea, o si tiene más de un
   * token.
   * @throw InvalidStateError Si q0 ∉ Q.
   */
  State ParseInitialStateSection(const std::set<State>& states);

  /**
   * @brief Lee la línea de Z0 y comprueba que pertenece a Γ.
   * @param stack_alphabet Alfabeto de pila (Γ), ya leído.
   * @throw MissingSectionError Si falta la línea, o si tiene más de un
   * token.
   * @throw InvalidSymbolError Si el token no tiene un único carácter, o
   * si Z0 ∉ Γ.
   */
  Symbol ParseInitialStackSymbolSection(const Alphabet& stack_alphabet);

  /**
   * @brief Lee la línea de F y comprueba que F ⊆ Q. Si la línea tiene
   * exactamente 5 tokens y no todos son estados ya declarados en Q, se
   * interpreta como la primera transición de un fichero pensado para un
   * autómata por vaciado de pila (APv, sin línea de F) y se informa de
   * ello explícitamente en vez de fallar con un error confuso. Es una
   * comprobación heurística, no infalible: un F con exactamente 5
   * estados, todos ya declarados en Q, no dispara este diagnóstico.
   * @param states Conjunto de estados (Q), ya leído.
   * @throw MissingSectionError Si falta la línea, o si el fichero parece
   * ser de formato APv.
   * @throw DuplicatedElementError Si algún estado aparece repetido en F.
   * @throw InvalidStateError Si algún estado de F no pertenece a Q.
   */
  std::vector<State> ParseFinalStatesSection(const std::set<State>& states);

  /**
   * @brief Construye y valida una transición a partir de su línea.
   * @param source_line Línea con los cinco campos de la transición.
   * @param states Conjunto de estados (Q), ya leído.
   * @param input_alphabet Alfabeto de entrada (Σ), ya leído.
   * @param stack_alphabet Alfabeto de pila (Γ), ya leído.
   * @throw InvalidTransitionError Si la línea no tiene exactamente cinco
   * campos, si algún campo que debe ser un único símbolo no lo es, si la
   * cima consultada es ε, o si algún estado o símbolo no está declarado.
   */
  Transition ParseTransitionLine(const SourceLine& source_line,
                                 const std::set<State>& states,
                                 const Alphabet& input_alphabet,
                                 const Alphabet& stack_alphabet);

  /**
   * @brief Comprueba que ningún estado aparece repetido en un vector de
   * estados recién leído (Q o F).
   * @throw DuplicatedElementError Si encuentra un estado repetido.
   */
  void CheckNoDuplicateStates(const std::vector<State>& states,
                              const std::string& section_name,
                              int line_number) const;

  std::string
      configuration_file_path_; /**< Ruta del fichero de configuración. */
  std::vector<SourceLine>
      significant_lines_; /**< Líneas significativas del fichero, en orden. */
  std::size_t current_line_index_ =
      0; /**< Índice de la próxima línea a consumir. */
};

#endif  // AUTOMATON_PARSER_H_