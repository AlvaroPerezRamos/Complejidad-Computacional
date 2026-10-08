/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 3: Funciones primitivas recursivas de números naturales
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 09/10/2026
 * @file command_line_options.cc
 * @brief Implementación de la clase CommandLineOptions.
 */

#include "../include/command_line_options.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <limits>
#include <sstream>
#include <system_error>

#include "../include/errors.h"

namespace {

/**
 * @brief Convierte el texto de una opción en un Natural.
 * @param option_name Opción a la que pertenece el valor (para los mensajes).
 * @param option_text Texto recibido.
 * @param help_text Ayuda que se añade al mensaje de error.
 * @throw CommandLineError Si no son solo dígitos o no cabe en Natural.
 */
Natural ParseNatural(const std::string& option_name,
                     const std::string& option_text,
                     const std::string& help_text) {
  // Solo dígitos decimales: se rechazan signos, decimales, espacios y texto.
  const bool contains_only_digits =
      !option_text.empty() &&
      std::all_of(
          option_text.begin(), option_text.end(),
          [](unsigned char character) { return std::isdigit(character) != 0; });
  if (!contains_only_digits) {
    throw CommandLineError(
        "El valor de '" + option_name +
        "' debe ser un número natural (solo dígitos) y se ha recibido '" +
        option_text + "'.\n\n" + help_text);
  }

  Natural parsed_value = 0;
  const char* text_begin = option_text.data();
  const char* text_end = option_text.data() + option_text.size();
  const std::from_chars_result conversion =
      std::from_chars(text_begin, text_end, parsed_value);
  if (conversion.ec == std::errc::result_out_of_range ||
      conversion.ptr != text_end) {
    throw CommandLineError("El valor de '" + option_name + "' ('" +
                           option_text +
                           "') no cabe en un natural de 64 bits (máximo " +
                           std::to_string(std::numeric_limits<Natural>::max()) +
                           ").\n\n" + help_text);
  }
  return parsed_value;
}

}  // namespace

std::string CommandLineOptions::BuildHelpText(const std::string& program_name) {
  std::ostringstream help_text;
  help_text
      << "Uso: " << program_name << " -x <n> -y <n>\n"
      << "\n"
      << "  -x <n>         Base de la potencia (natural). Obligatoria.\n"
      << "  -y <n>         Exponente de la potencia (natural). Obligatoria.\n"
      << "  -h, --help     Muestra esta ayuda y termina.\n";
  return help_text.str();
}

CommandLineOptions CommandLineOptions::Parse(int argc, char** argv) {
  CommandLineOptions options;
  const std::string program_name = (argc > 0) ? argv[0] : "potencia";
  const std::string help_text = BuildHelpText(program_name);

  bool has_base = false;
  bool has_exponent = false;

  int argument_index = 1;
  while (argument_index < argc) {
    const std::string current_argument = argv[argument_index];

    if (current_argument == "-h" || current_argument == "--help") {
      options.is_help_requested_ = true;
      return options;
    }

    const bool is_recognized_option =
        current_argument == "-x" || current_argument == "-y";
    if (!is_recognized_option) {
      throw CommandLineError("Opción desconocida: '" + current_argument +
                             "'.\n\n" + help_text);
    }

    if (argument_index + 1 >= argc) {
      throw CommandLineError("A la opción '" + current_argument +
                             "' le falta el valor.\n\n" + help_text);
    }
    const std::string option_value = argv[argument_index + 1];
    argument_index += 2;

    if (current_argument == "-x") {
      if (has_base) {
        throw CommandLineError("La opción '-x' está repetida.\n\n" + help_text);
      }
      has_base = true;
      options.base_ = ParseNatural(current_argument, option_value, help_text);
    } else {  // "-y"
      if (has_exponent) {
        throw CommandLineError("La opción '-y' está repetida.\n\n" + help_text);
      }
      has_exponent = true;
      options.exponent_ =
          ParseNatural(current_argument, option_value, help_text);
    }
  }

  if (!has_base) {
    throw CommandLineError("Falta la opción obligatoria '-x'.\n\n" + help_text);
  }
  if (!has_exponent) {
    throw CommandLineError("Falta la opción obligatoria '-y'.\n\n" + help_text);
  }
  return options;
}