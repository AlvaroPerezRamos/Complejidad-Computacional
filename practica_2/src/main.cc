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
 * @file main.cc
 * @brief Programa principal: CommandLineOptions -> TuringMachineParser ->
 * TuringMachineValidator -> Simulator. Cadenas desde -in o teclado.
 */

#include <fstream>
#include <iostream>
#include <string>

#include "../include/chain.h"
#include "../include/command_line_options.h"
#include "../include/errors.h"
#include "../include/simulator.h"
#include "../include/turing_machine.h"
#include "../include/turing_machine_parser.h"
#include "../include/turing_machine_validator.h"

namespace {

/** @brief Imprime Q, Σ, Γ, q0, b, F, número de cintas y número de transiciones.
 */
void PrintMachineSummary(const TuringMachine& machine) {
  std::cout << "Estados (Q): ";
  for (const State& state : machine.GetStateDeclarationOrder())
    std::cout << state << " ";
  std::cout << "\n";
  std::cout << "Alfabeto de entrada (Σ): " << machine.GetInputAlphabet()
            << "\n";
  std::cout << "Alfabeto de cinta (Γ): " << machine.GetTapeAlphabet() << "\n";
  std::cout << "Estado inicial (q0): " << machine.GetInitialState() << "\n";
  std::cout << "Símbolo blanco (b): " << machine.GetBlankSymbol() << "\n";
  std::cout << "Estados finales (F): ";
  for (const State& state : machine.GetFinalStates()) std::cout << state << " ";
  std::cout << "\n";
  std::cout << "Número de cintas: " << machine.GetTapeCount() << "\n";
  std::cout << "Transiciones: " << machine.GetTransitionFunction().Size()
            << "\n";
}

/**
 * @brief Comprueba, una por una, las cadenas de input_stream. Un
 * ChainError descarta esa cadena y sigue con la siguiente.
 * @param is_interactive Si es teclado: solo entonces se imprime "> " y
 * 'exit' termina antes de EOF.
 */
void RunChainLoop(const TuringMachine& machine, const Simulator& simulator,
                  std::istream& input_stream, bool is_interactive) {
  if (is_interactive) {
    std::cout << "\nIntroduce cadenas para comprobar (una por línea; '"
              << machine.GetBlankSymbol()
              << "' o una línea vacía para la cadena vacía; "
              << "'exit' o Ctrl+D para terminar):\n";
  }

  std::string line;
  while (true) {
    if (is_interactive) {
      std::cout << "> ";
      std::cout.flush();
    }
    if (!std::getline(input_stream, line)) break;
    if (is_interactive && line == "exit") break;

    try {
      const Chain chain(line, machine.GetInputAlphabet(),
                        machine.GetBlankSymbol());
      const TuringRunResult result = simulator.Run(chain);
      std::cout << (result.is_accepted ? "ACEPTADA" : "RECHAZADA") << "  "
                << result.tape_contents << "\n";
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
          argc > 0 ? argv[0] : "maquina_turing");
      return 0;
    }

    TuringMachineParser parser(options.GetConfigurationFilePath());
    const TuringMachine machine = parser.Parse(std::cout);
    TuringMachineValidator::Validate(machine, std::cout);
    PrintMachineSummary(machine);

    const Simulator simulator(machine);

    if (options.HasInputFile()) {
      std::ifstream input_file(options.GetInputFilePath());
      RunChainLoop(machine, simulator, input_file, /*is_interactive=*/false);
    } else {
      RunChainLoop(machine, simulator, std::cin, /*is_interactive=*/true);
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