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
 * @file main.cc
 * @brief Programa principal, versión provisional.
 *
 * Lee el fichero de configuración completo -Q, Σ, Γ, q0, Z0, F y las
 * transiciones-, ignorando comentarios y líneas en blanco, para validar
 * las clases ya implementadas (Symbol, State, Alphabet, Chain, Stack,
 * Transition y TransitionFunction). Todavía no construye un
 * PushdownAutomaton (no existe esa clase) ni comprueba las cadenas de
 * entrada: solo imprime lo que ha leído, para poder revisarlo. Tampoco
 * valida todavía las restricciones que exigen conocer el autómata
 * completo (p. ej. que q0 pertenezca a Q, que F esté contenido en Q, o
 * que los símbolos de las transiciones pertenezcan a Σ/Γ): esas
 * comprobaciones son responsabilidad de AutomatonValidator, que se
 * implementará más adelante. Por eso este fichero se sustituirá y no
 * forma parte todavía de la arquitectura final descrita en el README.
 *
 * Historial de versiones
 *   19/09/2026 - Creación: lectura de comentarios y líneas en blanco, y
 *                de las líneas de Q, Σ y Γ.
 *   19/09/2026 - Ampliación, el mismo día: lectura de q0, Z0, F y de las
 *                transiciones, con construcción de la TransitionFunction
 *                completa y numeración de las transiciones al imprimirlas.
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
#include "../include/symbol.h"
#include "../include/transition.h"
#include "../include/transition_function.h"

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
 * @brief Divide una línea en los tokens separados por espacios en blanco
 * que contiene.
 * @param line Línea a dividir.
 * @return Los tokens, en el orden en que aparecen en la línea.
 */
std::vector<std::string> SplitIntoTokens(const std::string& line) {
  std::vector<std::string> tokens;
  std::istringstream token_stream(line);
  std::string token;
  while (token_stream >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

/**
 * @brief Construye el conjunto de estados (Q, o también F) a partir de su
 * línea del fichero de configuración.
 * @param states_line Línea con los nombres de los estados, separados por
 * espacios.
 * @return Los estados en el orden en que aparecen en la línea.
 */
std::vector<State> ParseStates(const std::string& states_line) {
  std::vector<State> states;
  for (const std::string& token : SplitIntoTokens(states_line)) {
    states.emplace_back(token);
  }
  return states;
}

/**
 * @brief Construye un único estado a partir de su línea (usado para q0,
 * que debe ser exactamente un token).
 * @param line Línea con el nombre del estado inicial.
 * @throw MissingSectionError Si la línea no tiene exactamente un token
 * (el enunciado exige detectar "más de un estado inicial" como error).
 */
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

/**
 * @brief Construye un único símbolo a partir de su línea (usado para Z0,
 * que debe ser exactamente un token de un carácter).
 * @param line Línea con el símbolo inicial de la pila.
 * @throw MissingSectionError Si la línea no tiene exactamente un token.
 * @throw InvalidSymbolError Si el token no tiene un único carácter.
 */
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

/**
 * @brief Construye una Transition a partir de su línea del fichero de
 * configuración: origen, símbolo de entrada, símbolo de la cima, destino
 * y secuencia a apilar.
 * @param transition_line Línea con los cinco campos de la transición.
 * @throw InvalidTransitionError Si la línea no tiene exactamente cinco
 * campos, si el símbolo de entrada o la cima consultada no son un único
 * carácter, o si la cima consultada es ε (nunca puede serlo).
 */
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

    if (!ReadNextSignificantLine(config_file, line)) {
      throw MissingSectionError(
          "El fichero de configuración está "
          "incompleto: falta la línea de q0.");
    }
    const State initial_state = ParseSingleState(line);

    if (!ReadNextSignificantLine(config_file, line)) {
      throw MissingSectionError(
          "El fichero de configuración está "
          "incompleto: falta la línea de Z0.");
    }
    const Symbol initial_stack_symbol = ParseSingleSymbol(line);

    if (!ReadNextSignificantLine(config_file, line)) {
      throw MissingSectionError(
          "El fichero de configuración está "
          "incompleto: falta la línea de F.");
    }
    const std::vector<State> final_states = ParseStates(line);

    TransitionFunction transition_function;
    while (ReadNextSignificantLine(config_file, line)) {
      const Transition transition = ParseTransitionLine(line);
      if (!transition_function.Insert(transition)) {
        std::cout << "[Aviso] Transición duplicada, se ignora: " << transition
                  << "\n";
      }
    }

    std::cout << "Estados (Q): ";
    for (const State& state : states) std::cout << state << " ";
    std::cout << "\n";
    std::cout << "Alfabeto de entrada (Σ): " << input_alphabet << "\n";
    std::cout << "Alfabeto de pila (Γ): " << stack_alphabet << "\n";
    std::cout << "Estado inicial (q0): " << initial_state << "\n";
    std::cout << "Símbolo inicial de pila (Z0): " << initial_stack_symbol
              << "\n";
    std::cout << "Estados finales (F): ";
    for (const State& state : final_states) std::cout << state << " ";
    std::cout << "\n";

    std::cout << "Transiciones (" << transition_function.Size() << "):\n";
    std::size_t transition_number = 1;
    for (const Transition& transition :
         transition_function.GetOrderedTransitions(states)) {
      std::cout << "  " << transition_number++ << ". " << transition << "\n";
    }

  } catch (const Error& error) {
    std::cerr << "Error: " << error.what() << "\n";
    return 1;
  }

  return 0;
}