/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 23/09/2026
 * @file command_line_options.cc
 * @brief Implementación de la clase CommandLineOptions.
 */


#include <fstream>
#include <sstream>

#include "../include/command_line_options.h"
#include "../include/errors.h"

std::string CommandLineOptions::BuildHelpText(const std::string& program_name) {
  std::ostringstream help_text;
  help_text << "Uso: " << program_name
            << " -config <f> -trace <y|n> [-in <f>] [-out <f>]\n"
            << "\n"
            << "  -config <f>    Fichero de texto con la definición del "
               "autómata. Obligatoria.\n"
            << "  -trace <y|n>   Activa (y) o desactiva (n) el modo traza. "
               "Obligatoria.\n"
            << "  -in <f>        Fichero con las cadenas a comprobar. "
               "Opcional: si se omite,\n"
            << "                 las cadenas se leen por teclado.\n"
            << "  -out <f>       Fichero donde se almacena la traza. Opcional: "
               "si se omite,\n"
            << "                 la traza se muestra por pantalla.\n"
            << "  -h, --help     Muestra esta ayuda y termina.\n";
  return help_text.str();
}

CommandLineOptions CommandLineOptions::Parse(int argc, char** argv) {
  CommandLineOptions options;
  const std::string program_name = (argc > 0) ? argv[0] : "pda_simulator";

  bool has_config = false;
  bool has_trace = false;
  bool has_in = false;
  bool has_out = false;

  int argument_index = 1;
  while (argument_index < argc) {
    const std::string current_argument = argv[argument_index];

    if (current_argument == "-h" || current_argument == "--help") {
      options.is_help_requested_ = true;
      return options;  // Se pidió ayuda: no hace falta validar nada más.
    }

    const bool is_recognized_option =
        current_argument == "-config" || current_argument == "-trace" ||
        current_argument == "-in" || current_argument == "-out";
    if (!is_recognized_option) {
      throw CommandLineError("Opción desconocida: '" + current_argument +
                             "'.\n\n" + BuildHelpText(program_name));
    }

    if (argument_index + 1 >= argc) {
      throw CommandLineError("A la opción '" + current_argument +
                             "' le falta el valor.\n\n" +
                             BuildHelpText(program_name));
    }
    const std::string option_value = argv[argument_index + 1];
    argument_index += 2;

    if (current_argument == "-config") {
      if (has_config) {
        throw CommandLineError("La opción '-config' está repetida.\n\n" +
                               BuildHelpText(program_name));
      }
      has_config = true;
      options.configuration_file_path_ = option_value;

    } else if (current_argument == "-trace") {
      if (has_trace) {
        throw CommandLineError("La opción '-trace' está repetida.\n\n" +
                               BuildHelpText(program_name));
      }
      if (option_value != "y" && option_value != "n") {
        throw CommandLineError(
            "El valor de '-trace' debe ser 'y' o 'n'; se ha recibido '" +
            option_value + "'.\n\n" + BuildHelpText(program_name));
      }
      has_trace = true;
      options.is_trace_enabled_ = (option_value == "y");

    } else if (current_argument == "-in") {
      if (has_in) {
        throw CommandLineError("La opción '-in' está repetida.\n\n" +
                               BuildHelpText(program_name));
      }
      has_in = true;
      options.input_file_path_ = option_value;

    } else {  // "-out"
      if (has_out) {
        throw CommandLineError("La opción '-out' está repetida.\n\n" +
                               BuildHelpText(program_name));
      }
      has_out = true;
      options.output_file_path_ = option_value;
    }
  }

  if (!has_config || !has_trace) {
    throw CommandLineError(
        "Faltan opciones obligatorias: hacen falta '-config' y "
        "'-trace'.\n\n" +
        BuildHelpText(program_name));
  }

  const std::ifstream configuration_file_check(
      options.configuration_file_path_);
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

  if (has_out) {
    if (!options.is_trace_enabled_) {
      throw CommandLineError(
          "'-out' no tiene sentido junto con '-trace n': el fichero "
          "de traza quedaría vacío.");
    }
    if (options.output_file_path_ == options.configuration_file_path_ ||
        (has_in && options.output_file_path_ == options.input_file_path_)) {
      throw CommandLineError(
          "El fichero de '-out' no puede coincidir con un fichero de "
          "entrada ('-config' o '-in'): se destruiría.");
    }
    const std::ofstream output_file_check(options.output_file_path_);
    if (!output_file_check.is_open()) {
      throw CommandLineError("No se puede crear el fichero de '-out': '" +
                             options.output_file_path_ + "'.");
    }
  }

  return options;
}