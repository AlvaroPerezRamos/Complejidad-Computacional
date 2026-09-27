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
 * @brief Programa principal: CommandLineOptions -> AutomatonParser ->
 * AutomatonValidator -> Simulator. Cadenas desde -in o teclado; traza a
 * -out o pantalla.
 */

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>

#include "../include/command_line_options.h"
#include "../include/automaton_parser.h"
#include "../include/automaton_validator.h"
#include "../include/simulator.h"
#include "../include/errors.h"

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