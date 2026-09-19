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
 * transiciones-, usando las funciones de config_reader.h, para validar
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
 *              - Ampliación, el mismo día: lectura de q0, Z0, F y de las
 *                transiciones, con construcción de la TransitionFunction
 *                completa y numeración de las transiciones al imprimirlas.
 *              - Ampliación, el mismo día: se extraen a config_reader.h/
 *                .cc todas las funciones de lectura y parseo, dejando
 *                aquí únicamente el punto de entrada.
 */

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../include/alphabet.h"
#include "../include/config_reader.h"
#include "../include/errors.h"
#include "../include/state.h"
#include "../include/symbol.h"
#include "../include/transition.h"
#include "../include/transition_function.h"

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