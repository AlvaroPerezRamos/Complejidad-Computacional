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
 * @brief Jerarquía de excepciones del simulador.
 * 
 * Historial de versiones
 *   18/09/2026 - Creación del fichero y definición de la clase base Error
 *   19/09/2026 - Creación: Error, ConfigurationError y sus subclases
 *                MissingSectionError, InvalidSymbolError y
 *                DuplicatedElementError; y ChainError.
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#include <stdexcept>
#include <string>

/**
 * @class Error
 * @brief Clase base de todas las excepciones propias del simulador.
 */
class Error : public std::exception {
 public:
  /**
   * @brief Construye el error con el mensaje que se mostrará al usuario.
   * @param message Descripción del error.
   */
  explicit Error(const std::string& message) : message_(message) {}

  /** @brief Devuelve el mensaje de error como cadena de estilo C. */
  const char* what() const noexcept override { return message_.c_str(); }

 private:
  std::string message_; /**< Mensaje descriptivo del error. */
};

// =============================================================================
// Errores del fichero de configuración
// =============================================================================

/**
 * @class ConfigurationError
 * @brief Clase base de los errores detectados al leer el fichero de
 * configuración. Todos deben poder indicar el número de línea real del
 * fichero en el que se ha detectado el problema (comentarios y líneas en
 * blanco incluidos), tal y como exige el enunciado.
 *
 * El número de línea no siempre se conoce en el punto donde se detecta el
 * error: por ejemplo, Alphabet valida sus símbolos sin saber en qué línea
 * del fichero está. Por eso el número de línea tiene un valor por defecto
 * (-1, "todavía sin determinar") y puede completarse después con
 * SetLineNumber(), una vez que AutomatonParser -que sí conoce el contexto-
 * captura la excepción y la reenvía con la línea correcta.
 */
class ConfigurationError : public Error {
 public:
  /**
   * @brief Construye el error de configuración.
   * @param message Descripción del error.
   * @param line_number Línea del fichero donde se detectó, o -1 si aún no
   * se conoce.
   */
  explicit ConfigurationError(const std::string& message, int line_number = -1)
      : Error(message), line_number_(line_number) {}

  /** @brief Devuelve la línea del fichero asociada al error, o -1 si no se ha
   * fijado. */
  int GetLineNumber() const { return line_number_; }

  /** @brief Fija la línea del fichero una vez que se conoce el contexto. */
  void SetLineNumber(int line_number) { line_number_ = line_number; }

 private:
  int line_number_; /**< Línea del fichero de configuración, o -1 si es
                       desconocida. */
};

/**
 * @class MissingSectionError
 * @brief Falta una sección obligatoria del fichero de configuración (Q, Σ,
 * Γ, q0, Z0, F...), o el fichero termina antes de completarla.
 */
class MissingSectionError : public ConfigurationError {
 public:
  explicit MissingSectionError(const std::string& message, int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

/**
 * @class InvalidSymbolError
 * @brief Un símbolo de Σ o Γ no es válido: tiene más de un carácter, o es
 * el carácter reservado '.' (que representa ε).
 */
class InvalidSymbolError : public ConfigurationError {
 public:
  explicit InvalidSymbolError(const std::string& message, int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

/**
 * @class DuplicatedElementError
 * @brief Un símbolo o un estado aparece repetido en un conjunto que debe
 * tener elementos distintos (Σ, Γ, Q o F).
 */
class DuplicatedElementError : public ConfigurationError {
 public:
  explicit DuplicatedElementError(const std::string& message,
                                  int line_number = -1)
      : ConfigurationError(message, line_number) {}
};

// =============================================================================
// Errores de las cadenas de entrada
// =============================================================================

/**
 * @class ChainError
 * @brief Error propio de una cadena de entrada concreta, no del fichero de
 * configuración: según la sección 6 del enunciado, no aborta el programa,
 * solo descarta esa cadena y se continúa con la siguiente.
 */
class ChainError : public Error {
 public:
  explicit ChainError(const std::string& message) : Error(message) {}
};

#endif  // ERRORS_H_