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
 * @file command_line_options.h
 * @brief Definición de CommandLineOptions: -config (obligatoria), -in
 * (opcional), -h/--help. Mucho más simple que en P01: no hay -trace ni
 * -out, el enunciado de P02 no los pide.
 */

#ifndef COMMAND_LINE_OPTIONS_H_
#define COMMAND_LINE_OPTIONS_H_

#include <string>

class CommandLineOptions {
 public:
  /**
   * @return Opciones validadas, o con IsHelpRequested() a true si se pidió ayuda.
   * @throw CommandLineError Con el texto de ayuda incluido en el mensaje.
   */
  static CommandLineOptions Parse(int argc, char** argv);

  static std::string BuildHelpText(const std::string& program_name);

  bool IsHelpRequested() const { return is_help_requested_; }
  const std::string& GetConfigurationFilePath() const { return configuration_file_path_; }
  bool HasInputFile() const { return !input_file_path_.empty(); }
  const std::string& GetInputFilePath() const { return input_file_path_; }

 private:
  CommandLineOptions() = default;

  bool is_help_requested_ = false;
  std::string configuration_file_path_;
  std::string input_file_path_;
};

#endif  // COMMAND_LINE_OPTIONS_H_
