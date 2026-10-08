/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 3: Funciones primitivas recursivas de números naturales
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 08/10/2026
 * @file errors.h
 * @brief Jerarquía de excepciones del proyecto. Se amplía según lo van
 * necesitando las clases (mismo patrón que en P01 y P02): de momento solo
 * lo que necesitan PrimitiveRecursiveFunction y las funciones básicas.
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#include <exception>
#include <string>

/** @brief Clase base de todas las excepciones propias del proyecto. */
class Error : public std::exception {
 public:
  explicit Error(const std::string& message) : message_(message) {}
  ~Error() override = default;

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  std::string message_;
};

// =============================================================================
// Errores de las funciones
// =============================================================================

/** @brief Se ha evaluado una función con un número de argumentos distinto de su
 * aridad. */
class ArityMismatchError : public Error {
 public:
  explicit ArityMismatchError(const std::string& message) : Error(message) {}
};

/**
 * @brief Una función se ha construido con componentes incoherentes: una
 * proyección Pᵢⁿ con i fuera de [1, n] o, más adelante, una composición o una
 * recursión cuyas aridades no encajan.
 */
class InvalidFunctionDefinitionError : public Error {
 public:
  explicit InvalidFunctionDefinitionError(const std::string& message)
      : Error(message) {}
};

/** @brief Un valor sale del rango representable por Natural. */
class NaturalOverflowError : public Error {
 public:
  explicit NaturalOverflowError(const std::string& message) : Error(message) {}
};

#endif  // ERRORS_H_