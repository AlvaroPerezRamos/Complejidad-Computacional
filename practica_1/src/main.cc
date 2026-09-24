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
 * @brief Programa principal.
 *
 * Ya con CommandLineOptions real: -config y -trace obligatorios, -in y
 * -out opcionales, -h/--help para la ayuda. Las cadenas se leen de -in
 * si se especifica, o por teclado si no (una por línea, '.' para la
 * cadena vacía, 'exit' o Ctrl+D para terminar -esto último solo tiene
 * sentido en modo teclado, así que RunChainLoop() solo lo comprueba
 * cuando is_interactive es true-). La traza va a -out si se especifica,
 * o a pantalla si no.
 *
 * Historial de versiones
 *   19/09/2026 - Creación: lectura de comentarios y líneas en blanco, y
 *                de las líneas de Q, Σ y Γ.
 *   19/09/2026 - Ampliación: el mismo día: lectura de q0, Z0, F y de las
 *                transiciones, con construcción de la TransitionFunction
 *                completa y numeración de las transiciones al imprimirlas.
 *   19/09/2026 - Ampliación: se extraen a config_reader.h/
 *                .cc todas las funciones de lectura y parseo, dejando
 *                aquí únicamente el punto de entrada.
 *   19/09/2026 - Reescritura: config_reader.h/.cc
 *                desaparece, sustituido por AutomatonParser (que además
 *                valida las referencias cruzadas entre Q/Σ/Γ y fija el
 *                número de línea real de cada error). main.cc pasa a
 *                usar PushdownAutomaton en vez de variables sueltas.
 *   19/09/2026 - Ampliación: se llama a
 *                AutomatonValidator::Validate() justo después de
 *                construir el autómata, para los avisos que necesitan el
 *                grafo de transiciones completo.
 *   23/09/2026 - Ampliación: bucle de comprobación de cadenas por
 *                teclado con Simulator, ahora que existe.
 *   23/09/2026 - Ampliación: Tracer ya existe e integrado
 *                en Simulator; se añade un '-trace' opcional como tercer
 *                argumento (sin CommandLineOptions todavía, así que no
 *                es '-trace y|n' real: su sola presencia activa la
 *                traza).
 *   23/09/2026 - Reescritura: CommandLineOptions ya existe.
 *                Se sustituye el '-trace' ad hoc por las opciones reales
 *                del enunciado (-config/-trace/-in/-out/-h), con
 *                lectura de cadenas desde -in y traza hacia -out cuando
 *                se especifican.
 *   23/09/2026 - Ampliación: flush() explícito tras el
 *                prompt y tras el veredicto en RunChainLoop(), para que
 *                se vean al momento (ver también la explicación en
 *                tracer.h sobre BeginChain()/EndChain()).
 */

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

#include "../include/automaton_parser.h"
#include "../include/automaton_validator.h"
#include "../include/chain.h"
#include "../include/command_line_options.h"
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
 * @brief Comprueba, una por una, las cadenas que llegan por input_stream.
 * Un ChainError (símbolo fuera de Σ) o un SimulationLimitExceededError
 * descartan esa cadena y se continúa con la siguiente, tal y como exige
 * la sección 6 del enunciado; no abortan el programa.
 * @param automaton Autómata ya construido.
 * @param simulator Simulador a usar (ya construido con el ostream y el
 * booleano de traza correctos).
 * @param input_stream De dónde leer las cadenas: std::cin o el fichero
 * de -in.
 * @param is_interactive Si input_stream es el teclado: solo entonces se
 * imprime el símbolo de espera ("> ") y solo entonces 'exit' termina el
 * bucle antes de llegar a EOF (no tiene sentido para un fichero de -in).
 * @param trace_enabled Si la traza está activada: si lo está, el
 * veredicto ya lo dice Tracer::EndChain(), así que no se repite aquí.
 */
void RunChainLoop(const PushdownAutomaton& automaton, Simulator& simulator,
                  std::istream& input_stream, bool is_interactive,
                  bool trace_enabled) {
  if (is_interactive) {
    std::cout << "\nIntroduce cadenas para comprobar (una por línea; '.' para "
                 "la cadena vacía; 'exit' o Ctrl+D para terminar):\n";
  }

  std::string line;
  while (true) {
    if (is_interactive) {
      std::cout << "> ";
      std::cout.flush();  // Sin esto, el "> " puede quedar sin mostrarse hasta
                          // el siguiente flush.
    }
    if (!std::getline(input_stream, line)) break;
    if (is_interactive && line == "exit") break;

    try {
      const Chain chain(line, automaton.GetInputAlphabet());
      const bool accepted = simulator.Accepts(chain);
      if (!trace_enabled) {
        std::cout << (accepted ? "ACEPTADA" : "RECHAZADA") << "\n";
        std::cout.flush();
      }
    } catch (const Error& error) {
      std::cerr << "Error: " << error.what() << "\n";
    }
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    const CommandLineOptions options = CommandLineOptions::Parse(argc, argv);

    if (options.IsHelpRequested()) {
      std::cout << CommandLineOptions::BuildHelpText(
          argc > 0 ? argv[0] : "pda_simulator");
      return 0;
    }

    AutomatonParser parser(options.GetConfigurationFilePath());
    const PushdownAutomaton automaton = parser.Parse(std::cout);
    AutomatonValidator::Validate(automaton, std::cout);
    PrintAutomatonSummary(automaton);

    std::ofstream trace_output_file;
    std::ostream* trace_stream = &std::cout;
    if (options.HasOutputFile()) {
      trace_output_file.open(options.GetOutputFilePath());
      trace_stream = &trace_output_file;
    }

    Simulator simulator(automaton, *trace_stream, options.IsTraceEnabled());

    if (options.HasInputFile()) {
      std::ifstream input_file(options.GetInputFilePath());
      RunChainLoop(automaton, simulator, input_file, /*is_interactive=*/false,
                   options.IsTraceEnabled());
    } else {
      RunChainLoop(automaton, simulator, std::cin, /*is_interactive=*/true,
                   options.IsTraceEnabled());
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