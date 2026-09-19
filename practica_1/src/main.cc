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
 * Usa AutomatonParser para construir el PushdownAutomaton completo a
 * partir del fichero de configuración, con todas las comprobaciones de
 * la sección 6 del enunciado que abortan la carga. Todavía no comprueba
 * cadenas de entrada (no existe Simulator) ni ejecuta los avisos que
 * necesitan el autómata completo (no existe AutomatonValidator): solo
 * imprime lo que se ha leído, incluidos los estados alcanzables desde
 * q0, para poder revisarlo. Por eso este fichero se sustituirá y no
 * forma parte todavía de la arquitectura final descrita en el README.
 *
 * Historial de versiones
 *   19/09/2026 - Creación: lectura de comentarios y líneas en blanco, y
 *                de las líneas de Q, Σ y Γ.
 *   19/09/2026 - Ampliación, el mismo día: lectura de q0, Z0, F y de las
 *                transiciones, con construcción de la TransitionFunction
 *                completa y numeración de las transiciones al imprimirlas.
 *   19/09/2026 - Ampliación, el mismo día: se extraen a config_reader.h/
 *                .cc todas las funciones de lectura y parseo, dejando
 *                aquí únicamente el punto de entrada.
 *   19/09/2026 - Reescritura, el mismo día: config_reader.h/.cc
 *                desaparece, sustituido por AutomatonParser (que además
 *                valida las referencias cruzadas entre Q/Σ/Γ y fija el
 *                número de línea real de cada error). main.cc pasa a
 *                usar PushdownAutomaton en vez de variables sueltas.
 *   19/09/2026 - Ampliación, el mismo día: se llama a
 *                AutomatonValidator::Validate() justo después de
 *                construir el autómata, para los avisos que necesitan el
 *                grafo de transiciones completo.
 */

#include <cstddef>
#include <iostream>

#include "../include/automaton_parser.h"
#include "../include/automaton_validator.h"
#include "../include/errors.h"
#include "../include/pushdown_automaton.h"
#include "../include/state.h"
#include "../include/transition.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <fichero_configuracion>\n";
    return 1;
  }

  try {
    AutomatonParser parser(argv[1]);
    const PushdownAutomaton automaton = parser.Parse(std::cout);
    AutomatonValidator::Validate(automaton, std::cout);

    std::cout << "Estados (Q): ";
    for (const State& state : automaton.GetStateDeclarationOrder())
      std::cout << state << " ";
    std::cout << "\n";
    std::cout << "Alfabeto de entrada (Σ): " << automaton.GetInputAlphabet()
              << "\n";
    std::cout << "Alfabeto de pila (Γ): " << automaton.GetStackAlphabet()
              << "\n";
    std::cout << "Estado inicial (q0): " << automaton.GetInitialState() << "\n";
    std::cout << "Símbolo inicial de pila (Z0): "
              << automaton.GetInitialStackSymbol() << "\n";
    std::cout << "Estados finales (F): ";
    for (const State& state : automaton.GetFinalStates())
      std::cout << state << " ";
    std::cout << "\n";
    std::cout << "Estados alcanzables desde q0: ";
    for (const State& state : automaton.ComputeReachableStates())
      std::cout << state << " ";
    std::cout << "\n";

    const TransitionFunction& transition_function =
        automaton.GetTransitionFunction();
    std::cout << "Transiciones (" << transition_function.Size() << "):\n";
    std::size_t transition_number = 1;
    for (const Transition& transition :
         transition_function.GetOrderedTransitions(
             automaton.GetStateDeclarationOrder())) {
      std::cout << "  " << transition_number++ << ". " << transition << "\n";
    }

  } catch (const ConfigurationError& error) {
    std::cerr << "Error (línea " << error.GetLineNumber()
              << "): " << error.what() << "\n";
    return 1;
  } catch (const Error& error) {
    std::cerr << "Error: " << error.what() << "\n";
    return 1;
  }

  return 0;
}