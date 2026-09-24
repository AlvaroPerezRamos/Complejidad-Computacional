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
 * @file command_line_options.h
 * @brief Definición de la clase CommandLineOptions.
 *
 * Analiza y valida los argumentos de la línea de comandos según la
 * sección 6 del enunciado: -config y -trace obligatorios, -in y -out
 * opcionales, -h/--help para la ayuda. Todas las comprobaciones de esa
 * tabla ("Línea de comandos") viven aquí: opción repetida, sin valor,
 * desconocida, valor de -trace inválido, ficheros de -config/-in
 * inaccesibles, -out no creable, -out junto a -trace n, y -out
 * coincidiendo con un fichero de entrada (-config o -in).
 *
 * -h/--help no es un error: si aparece en cualquier posición, Parse()
 * devuelve inmediatamente unas opciones con IsHelpRequested() a true, sin
 * comprobar el resto (no tendría sentido exigir -config y -trace solo
 * para poder pedir ayuda).
 *
 * El constructor es privado: solo Parse() puede construir unas opciones
 * completas, para no dejar la clase en un estado a medio validar desde
 * fuera.
 *
 * Historial de versiones
 *   23/09/2026 - Creación del fichero e implementación completa.
 */

#ifndef COMMAND_LINE_OPTIONS_H_
#define COMMAND_LINE_OPTIONS_H_

#include <string>

/**
 * @class CommandLineOptions
 * @brief Representa los argumentos de la línea de comandos, ya
 * analizados y validados.
 */
class CommandLineOptions {
 public:
  /**
   * @brief Analiza y valida los argumentos de la línea de comandos.
   * @param argc Número de argumentos (incluyendo el nombre del programa).
   * @param argv Argumentos.
   * @return Las opciones ya validadas, o unas opciones con
   * IsHelpRequested() a true si se pidió ayuda.
   * @throw CommandLineError Si los argumentos incumplen cualquiera de las
   * restricciones de la sección 6 del enunciado. El mensaje incluye el
   * texto de ayuda.
   */
  static CommandLineOptions Parse(int argc, char** argv);

  /** @brief Construye el texto de ayuda (uso del programa y sus opciones). */
  static std::string BuildHelpText(const std::string& program_name);

  /** @brief Indica si se pidió ayuda (-h o --help). Si es true, el resto de los
   * campos no están validados. */
  bool IsHelpRequested() const { return is_help_requested_; }

  /** @brief Devuelve la ruta del fichero de configuración (-config). */
  const std::string& GetConfigurationFilePath() const {
    return configuration_file_path_;
  }

  /** @brief Indica si el modo traza está activado (-trace y). */
  bool IsTraceEnabled() const { return is_trace_enabled_; }

  /** @brief Indica si se especificó un fichero de cadenas de entrada (-in). */
  bool HasInputFile() const { return !input_file_path_.empty(); }

  /** @brief Devuelve la ruta del fichero de cadenas de entrada (-in), si se
   * especificó. */
  const std::string& GetInputFilePath() const { return input_file_path_; }

  /** @brief Indica si se especificó un fichero de salida para la traza (-out).
   */
  bool HasOutputFile() const { return !output_file_path_.empty(); }

  /** @brief Devuelve la ruta del fichero de salida para la traza (-out), si se
   * especificó. */
  const std::string& GetOutputFilePath() const { return output_file_path_; }

 private:
  /** @brief Constructor por defecto, privado: solo Parse() construye opciones
   * completas. */
  CommandLineOptions() = default;

  bool is_help_requested_ = false;      /**< Si se pidió -h o --help. */
  std::string configuration_file_path_; /**< Ruta de -config. */
  std::string
      input_file_path_; /**< Ruta de -in, o vacía si no se especificó. */
  std::string
      output_file_path_; /**< Ruta de -out, o vacía si no se especificó. */
  bool is_trace_enabled_ = false; /**< Si -trace es 'y'. */
};

#endif  // COMMAND_LINE_OPTIONS_H_