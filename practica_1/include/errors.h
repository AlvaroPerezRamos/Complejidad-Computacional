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
 * Este fichero contiene, por ahora, las excepciones que necesitan
 * Alphabet, Chain, Stack y la lectura de transiciones. El resto de la
 * jerarquía (errores de línea de comandos, de ficheros, de estados no
 * declarados...) se añadirá cuando se implementen las clases que las
 * lanzan.
 *
 * Historial de versiones
 *   18/09/2026 - Creación del fichero. Definición de la clase base Error:
 *                constructor y captador what().
 *   19/09/2026 - Ampliación con ConfigurationError y sus subclases
 *                (MissingSectionError, InvalidSymbolError,
 *                DuplicatedElementError), y con ChainError; necesarias
 *                para Alphabet y Chain respectivamente.
 *   19/09/2026 - Ampliación, el mismo día, con SimulationError y
 *                EmptyStackError, necesarias para Stack.
 *   19/09/2026 - Ampliación, el mismo día, con InvalidTransitionError,
 *                necesaria para leer las transiciones del fichero de
 *                configuración.
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

  /** @brief Destructor por defecto. */
  ~Error() override = default;

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
 * SetLineNumber(), una vez que quien construye el autómata a partir del
 * fichero -y sí conoce el contexto- captura la excepción y la reenvía con
 * la línea correcta.
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

/**
 * @class InvalidTransitionError
 * @brief Una línea de transición no es válida: no tiene exactamente cinco
 * campos, algún campo que debe ser un único símbolo no lo es, o la cima
 * consultada es ε (el enunciado exige que una transición nunca consulte
 * ε en la pila).
 */
class InvalidTransitionError : public ConfigurationError {
 public:
  explicit InvalidTransitionError(const std::string& message,
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

// =============================================================================
// Errores de la simulación
// =============================================================================

/**
 * @class SimulationError
 * @brief Clase base de los errores que pueden ocurrir mientras se explora
 * el árbol de descripciones instantáneas de una cadena concreta. Igual que
 * ChainError, no aborta el programa: solo esa cadena se descarta.
 */
class SimulationError : public Error {
 public:
  explicit SimulationError(const std::string& message) : Error(message) {}
};

/**
 * @class EmptyStackError
 * @brief Se ha intentado consultar (Top()) o desapilar (Pop()) la cima de
 * una pila vacía. En un uso correcto del simulador no debería llegar a
 * lanzarse nunca: una pila vacía significa que no hay cima que consultar,
 * así que Simulator debe comprobar Stack::IsEmpty() antes de explorar más
 * transiciones, no dejar que Stack detecte el problema. Se lanza de todos
 * modos como salvaguarda defensiva ante un error de programación.
 */
class EmptyStackError : public SimulationError {
 public:
  explicit EmptyStackError(const std::string& message)
      : SimulationError(message) {}
};

#endif  // ERRORS_H_