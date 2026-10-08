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
 * @file command_line_options.h
 * @brief Definición de CommandLineOptions: -x <n> (base) y -y <n> (exponente),
 * ambas obligatorias, y -h/--help. Mismo patrón que en P01 y P02.
 */

#ifndef COMMAND_LINE_OPTIONS_H_
#define COMMAND_LINE_OPTIONS_H_

#include <string>

#include "types.h"

class CommandLineOptions {
 public:
  /**
   * @return Opciones validadas, o con IsHelpRequested() a true si se pidió
   * ayuda.
   * @throw CommandLineError Con el texto de ayuda incluido en el mensaje.
   */
  static CommandLineOptions Parse(int argc, char** argv);

  static std::string BuildHelpText(const std::string& program_name);

  bool IsHelpRequested() const { return is_help_requested_; }
  Natural GetBase() const { return base_; }
  Natural GetExponent() const { return exponent_; }

 private:
  CommandLineOptions() = default;

  bool is_help_requested_ = false;
  Natural base_ = 0;
  Natural exponent_ = 0;
};

#endif  // COMMAND_LINE_OPTIONS_H_