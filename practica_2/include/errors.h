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
 * @file errors.h
 * @brief Jerarquía de excepciones del simulador. Se amplía según lo van
 * necesitando las clases; ver AutomataPila.md (P01) para el porqué de
 * este patrón de ampliación incremental.
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#include <stdexcept>
#include <string>

/** @brief Clase base de todas las excepciones propias del simulador. */
class Error : public std::exception {
 public:
  explicit Error(const std::string& message) : message_(message) {}
  ~Error() override = default;

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  std::string message_;
};

// =============================================================================
// Errores del fichero de configuración
// =============================================================================

/**
 * @brief Errores detectados al leer el fichero de configuración. Llevan el
 * número de línea real; -1 si aún no se conoce (ver SetLineNumber()).
 */
class ConfigurationError : public Error {
 public:
  explicit ConfigurationError(const std::string& message, int line_number = -1)
      : Error(message), line_number_(line_number) {}

  int GetLineNumber() const { return line_number_; }
  void SetLineNumber(int line_number) { line_number_ = line_number; }

 private:
  int line_number_;
};

/** @brief Falta una sección obligatoria o el fichero termina antes de completarla. */
class MissingSectionError : public ConfigurationError {
 public:
  explicit MissingSectionError(const std::string& message, int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

/** @brief Un símbolo no es válido: más de un carácter, o reservado (el blanco) donde no debe estar. */
class InvalidSymbolError : public ConfigurationError {
 public:
  explicit InvalidSymbolError(const std::string& message, int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

/** @brief Un símbolo o estado aparece repetido en un conjunto que debe tener elementos distintos. */
class DuplicatedElementError : public ConfigurationError {
 public:
  explicit DuplicatedElementError(const std::string& message, int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

// =============================================================================
// Errores de fichero
// =============================================================================

/** @brief El fichero de configuración no se puede abrir, o está vacío. No lleva número de línea. */
class FileError : public Error {
 public:
  explicit FileError(const std::string& message) : Error(message) {}
};

// =============================================================================
// Errores de la línea de comandos
// =============================================================================

/** @brief Argumentos de la línea de comandos inválidos. */
class CommandLineError : public Error {
 public:
  explicit CommandLineError(const std::string& message) : Error(message) {}
};

// =============================================================================
// Errores de las cadenas de entrada
// =============================================================================

/** @brief Error de una cadena de entrada concreta: no aborta el programa, solo descarta esa cadena. */
class ChainError : public Error {
 public:
  explicit ChainError(const std::string& message) : Error(message) {}
};

#endif  // ERRORS_H_
