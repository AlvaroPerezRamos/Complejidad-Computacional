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
 * Usa AutomatonParser para construir el PushdownAutomaton, lo pasa por
 * AutomatonValidator, y ya deja comprobar cadenas de verdad con
 * Simulator: se leen por teclado, una por línea ('.' para la cadena
 * vacía, 'exit' o Ctrl+D para terminar), tal y como describe la sección
 * 2 del enunciado para el modo teclado.
 *
 * Sigue siendo provisional: no acepta todavía las opciones reales de la
 * línea de comandos (-config/-trace/-in/-out), solo el fichero de
 * configuración como argumento único, y siempre lee las cadenas por
 * teclado (no hay -in ni -out porque no existe CommandLineOptions). Y
 * como tampoco existe Tracer, no hay modo traza: Simulator solo dice si
 * acepta o rechaza. Por eso este fichero se sustituirá y no forma parte
 * todavía de la arquitectura final descrita en el README.
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
 *   23/09/2026 - Ampliación: bucle de comprobación de cadenas por
 *                teclado con Simulator, ahora que existe.
 *   23/09/2026 - Ampliación, el mismo día: Tracer ya existe e integrado
 *                en Simulator; se añade un '-trace' opcional como tercer
 *                argumento (sin CommandLineOptions todavía, así que no
 *                es '-trace y|n' real: su sola presencia activa la
 *                traza).
 */

#include <cstddef>
#include <iostream>
#include <string>

#include "../include/automaton_parser.h"
#include "../include/automaton_validator.h"
#include "../include/chain.h"
#include "../include/errors.h"
#include "../include/pushdown_automaton.h"
#include "../include/simulator.h"
#include "../include/state.h"
#include "../include/transition.h"

namespace {

/**
 * @brief Imprime el resumen del autómata ya construido: Q, Σ, Γ, q0, Z0,
 * F, los estados alcanzables desde q0 y las transiciones numeradas.
 */
void PrintAutomatonSummary(const PushdownAutomaton& automaton) {
  std::cout << "Estados (Q): ";
  for (const State& state : automaton.GetStateDeclarationOrder())
    std::cout << state << " ";
  std::cout << "\n";
  std::cout << "Alfabeto de entrada (Σ): " << automaton.GetInputAlphabet()
            << "\n";
  std::cout << "Alfabeto de pila (Γ): " << automaton.GetStackAlphabet() << "\n";
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
  for (const Transition& transition : transition_function.GetOrderedTransitions(
           automaton.GetStateDeclarationOrder())) {
    std::cout << "  " << transition_number++ << ". " << transition << "\n";
  }
}

/**
 * @brief Bucle de comprobación de cadenas por teclado: una por línea,
 * '.' para la cadena vacía, 'exit' o Ctrl+D para terminar. Un ChainError
 * (símbolo fuera de Σ) o un SimulationLimitExceededError descartan esa
 * cadena y se continúa con la siguiente, tal y como exige la sección 6
 * del enunciado; no abortan el programa.
 */
void RunInteractiveLoop(const PushdownAutomaton& automaton,
                        bool trace_enabled) {
  Simulator simulator(automaton, std::cout, trace_enabled);

  std::cout << "\nIntroduce cadenas para comprobar (una por línea; '.' para "
               "la cadena vacía; 'exit' o Ctrl+D para terminar):\n";

  std::string line;
  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, line) || line == "exit") {
      break;
    }
    try {
      const Chain chain(line, automaton.GetInputAlphabet());
      const bool accepted = simulator.Accepts(chain);
      if (!trace_enabled) {
        std::cout << (accepted ? "ACEPTADA" : "RECHAZADA") << "\n";
      }
    } catch (const Error& error) {
      std::cerr << "Error: " << error.what() << "\n";
    }
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc != 2 && argc != 3) {
    std::cerr << "Uso: " << argv[0] << " <fichero_configuracion> [-trace]\n";
    return 1;
  }
  const bool trace_enabled = (argc == 3 && std::string(argv[2]) == "-trace");

  try {
    AutomatonParser parser(argv[1]);
    const PushdownAutomaton automaton = parser.Parse(std::cout);
    AutomatonValidator::Validate(automaton, std::cout);

    PrintAutomatonSummary(automaton);
    RunInteractiveLoop(automaton, trace_enabled);

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