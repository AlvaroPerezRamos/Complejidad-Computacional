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
 * lo que necesita PrimitiveRecursiveFunction.
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

#endif  // ERRORS_H_
