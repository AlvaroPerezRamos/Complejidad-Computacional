/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 18/09/2026
 * @file errors.h
 * @brief Jerarquía de excepciones del simulador. Ver AutomataPila.md para
 * la relación completa de qué lanza cada una y cuándo.
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
 * número de línea real del fichero; -1 si aún no se conoce (ver
 * SetLineNumber()).
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

/** @brief Falta una sección obligatoria (Q, Σ, Γ, q0, Z0, F...) o el fichero
 * termina antes de completarla. */
class MissingSectionError : public ConfigurationError {
 public:
  explicit MissingSectionError(const std::string& message, int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

/** @brief Un símbolo de Σ, Γ o Z0 no es válido: más de un carácter, '.', o no
 * declarado en su alfabeto. */
class InvalidSymbolError : public ConfigurationError {
 public:
  explicit InvalidSymbolError(const std::string& message, int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

/** @brief Un símbolo o estado aparece repetido en Σ, Γ, Q o F. */
class DuplicatedElementError : public ConfigurationError {
 public:
  explicit DuplicatedElementError(const std::string& message,
                                  int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

/** @brief Una transición no es válida: campos incorrectos, cima = ε, o algún
 * estado/símbolo no declarado. */
class InvalidTransitionError : public ConfigurationError {
 public:
  explicit InvalidTransitionError(const std::string& message,
                                  int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

/** @brief q0 ∉ Q, o algún estado de F ∉ Q. */
class InvalidStateError : public ConfigurationError {
 public:
  explicit InvalidStateError(const std::string& message, int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

// =============================================================================
// Errores de fichero
// =============================================================================

/** @brief El fichero de configuración no se puede abrir, o está vacío. No lleva
 * número de línea. */
class FileError : public Error {
 public:
  explicit FileError(const std::string& message) : Error(message) {}
};

// =============================================================================
// Errores de la línea de comandos
// =============================================================================

/** @brief Argumentos de la línea de comandos inválidos (ver la tabla de la
 * sección 6 del enunciado). */
class CommandLineError : public Error {
 public:
  explicit CommandLineError(const std::string& message) : Error(message) {}
};

// =============================================================================
// Errores de las cadenas de entrada
// =============================================================================

/** @brief Error de una cadena de entrada concreta: no aborta el programa, solo
 * descarta esa cadena. */
class ChainError : public Error {
 public:
  explicit ChainError(const std::string& message) : Error(message) {}
};

// =============================================================================
// Errores de la simulación
// =============================================================================

/** @brief Errores durante la exploración de una cadena concreta. Como
 * ChainError, no aborta el programa. */
class SimulationError : public Error {
 public:
  explicit SimulationError(const std::string& message) : Error(message) {}
};

/** @brief Top()/Pop() sobre una pila vacía. Salvaguarda defensiva: no debería
 * lanzarse en uso correcto. */
class EmptyStackError : public SimulationError {
 public:
  explicit EmptyStackError(const std::string& message)
      : SimulationError(message) {}
};

/** @brief Se ha superado alguna salvaguarda de Simulator (pila, descripciones
 * exploradas, o recursión). */
class SimulationLimitExceededError : public SimulationError {
 public:
  explicit SimulationLimitExceededError(const std::string& message)
      : SimulationError(message) {}
};

#endif  // ERRORS_H_