/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 3º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 19/09/2026
 * @file main.cc
 * @brief Programa principal, versión provisional.
 *
 * De momento solo comprueba la lectura de las tres primeras líneas
 * significativas del fichero de configuración -Q, Σ y Γ-, ignorando
 * comentarios y líneas en blanco, para validar las clases ya
 * implementadas (Symbol, State, Alphabet y Chain). La lectura de q0, Z0,
 * F y de las transiciones -y con ella la construcción real del
 * autómata- se añadirá cuando existan las clases correspondientes
 * (PushdownAutomaton, Transition, AutomatonParser...); por eso este
 * fichero se sustituirá más adelante y no forma parte todavía de la
 * arquitectura final descrita en el README.
 *
 * Historial de versiones
 *   19/09/2026 - Creación: lectura de comentarios y líneas en blanco, y
 *                de las líneas de Q, Σ y Γ.
 */

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/alphabet.h"
#include "../include/errors.h"
#include "../include/state.h"

/**
 * @brief Elimina el comentario de una línea del fichero de configuración:
 * todo lo que sigue al primer '#', según el convenio de la sección 3 del
 * enunciado.
 * @param line Línea tal y como se ha leído del fichero.
 * @return La línea sin su comentario.
 */
std::string StripComment(const std::string& line) {
  const std::size_t comment_position = line.find('#');
  return comment_position == std::string::npos
             ? line
             : line.substr(0, comment_position);
}

/**
 * @brief Indica si una línea, una vez eliminado su comentario, no
 * contiene ningún carácter que no sea espacio en blanco.
 * @param line Línea a comprobar.
 * @return true si la línea está en blanco, false en caso contrario.
 */
bool IsBlankLine(const std::string& line) {
  return line.find_first_not_of(" \t\r\n") == std::string::npos;
}

/**
 * @brief Lee la siguiente línea significativa del fichero, saltando
 * comentarios y líneas en blanco.
 * @param config_file Fichero de configuración abierto.
 * @param significant_line Donde se deja la línea leída (sin comentario).
 * @return true si se ha encontrado una línea significativa, false si se
 * ha llegado al final del fichero.
 */
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

/**
 * @brief Construye el conjunto de estados (Q) a partir de su línea del
 * fichero de configuración.
 * @param states_line Línea con los nombres de los estados, separados por
 * espacios.
 * @return Los estados en el orden en que aparecen en la línea.
 */
std::vector<State> ParseStates(const std::string& states_line) {
  std::vector<State> states;
  std::istringstream token_stream(states_line);
  std::string token;
  while (token_stream >> token) {
    states.emplace_back(token);
  }
  return states;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <fichero_configuracion>\n";
    return 1;
  }

  std::ifstream config_file(argv[1]);
  if (!config_file.is_open()) {
    std::cerr << "Error: no se puede abrir el fichero '" << argv[1] << "'.\n";
    return 1;
  }

  try {
    std::string line;

    if (!ReadNextSignificantLine(config_file, line)) {
      throw MissingSectionError(
          "El fichero de configuración está vacío o "
          "incompleto: falta la línea de Q.");
    }
    const std::vector<State> states = ParseStates(line);

    if (!ReadNextSignificantLine(config_file, line)) {
      throw MissingSectionError(
          "El fichero de configuración está "
          "incompleto: falta la línea de Σ.");
    }
    const Alphabet input_alphabet(line);

    if (!ReadNextSignificantLine(config_file, line)) {
      throw MissingSectionError(
          "El fichero de configuración está "
          "incompleto: falta la línea de Γ.");
    }
    const Alphabet stack_alphabet(line);

    std::cout << "Estados (Q): ";
    for (const State& state : states) std::cout << state << " ";
    std::cout << "\n";
    std::cout << "Alfabeto de entrada (Σ): " << input_alphabet << "\n";
    std::cout << "Alfabeto de pila (Γ): " << stack_alphabet << "\n";

  } catch (const Error& error) {
    std::cerr << "Error: " << error.what() << "\n";
    return 1;
  }

  return 0;
}