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
 * @file command_line_options.cc
 * @brief Implementación de la clase CommandLineOptions.
 */

#include "../include/command_line_options.h"

#include <fstream>
#include <sstream>

#include "../include/errors.h"

std::string CommandLineOptions::BuildHelpText(const std::string& program_name) {
  std::ostringstream help_text;
  help_text << "Uso: " << program_name << " -config <f> [-in <f>]\n"
      << "\n"
      << "  -config <f>    Fichero de texto con la configuración de la MT. Obligatoria.\n"
      << "  -in <f>        Fichero con las cadenas a comprobar. Opcional: si se omite,\n"
      << "                 las cadenas se leen por teclado.\n"
      << "  -h, --help     Muestra esta ayuda y termina.\n";
  return help_text.str();
}

CommandLineOptions CommandLineOptions::Parse(int argc, char** argv) {
  CommandLineOptions options;
  const std::string program_name = (argc > 0) ? argv[0] : "maquina_turing";

  bool has_config = false;
  bool has_in = false;

  int argument_index = 1;
  while (argument_index < argc) {
    const std::string current_argument = argv[argument_index];

    if (current_argument == "-h" || current_argument == "--help") {
      options.is_help_requested_ = true;
      return options;
    }

    const bool is_recognized_option = current_argument == "-config" || current_argument == "-in";
    if (!is_recognized_option) {
      throw CommandLineError("Opción desconocida: '" + current_argument + "'.\n\n" +
          BuildHelpText(program_name));
    }

    if (argument_index + 1 >= argc) {
      throw CommandLineError("A la opción '" + current_argument + "' le falta el valor.\n\n" +
          BuildHelpText(program_name));
    }
    const std::string option_value = argv[argument_index + 1];
    argument_index += 2;

    if (current_argument == "-config") {
      if (has_config) {
        throw CommandLineError("La opción '-config' está repetida.\n\n" + BuildHelpText(program_name));
      }
      has_config = true;
      options.configuration_file_path_ = option_value;
    } else {  // "-in"
      if (has_in) {
        throw CommandLineError("La opción '-in' está repetida.\n\n" + BuildHelpText(program_name));
      }
      has_in = true;
      options.input_file_path_ = option_value;
    }
  }

  if (!has_config) {
    throw CommandLineError("Falta la opción obligatoria '-config'.\n\n" + BuildHelpText(program_name));
  }

  const std::ifstream configuration_file_check(options.configuration_file_path_);
  if (!configuration_file_check.is_open()) {
    throw CommandLineError("No se puede acceder al fichero de '-config': '" +
        options.configuration_file_path_ + "'.");
  }

  if (has_in) {
    const std::ifstream input_file_check(options.input_file_path_);
    if (!input_file_check.is_open()) {
      throw CommandLineError("No se puede acceder al fichero de '-in': '" +
          options.input_file_path_ + "'.");
    }
  }

  return options;
}
