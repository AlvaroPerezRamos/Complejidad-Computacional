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
 * @file symbol.h
 * @brief Definición de la clase Symbol.
 *
 * Historial de versiones
 *   18/09/2026 - Creación del fichero. Esqueleto de la clase Symbol:
 *                constructor, destructor y captador GetCharacter().
 *   19/09/2026 - Implementación de IsEpsilon(), ToString() y de los
 *                operadores de comparación y de flujo.
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <iostream>
#include <string>

/**
 * @brief Carácter reservado que representa la cadena vacía (ε) tanto en el
 * fichero de configuración como en las cadenas de entrada. Ningún alfabeto
 * (Σ o Γ) puede contenerlo (ver Alphabet).
 */
constexpr char kEpsilonCharacter = '.';

/**
 * @class Symbol
 * @brief Representa un único símbolo: bien de un alfabeto (Σ o Γ), bien el
 * símbolo especial ε.
 */
class Symbol {
 public:
  /**
   * @brief Construye un símbolo a partir de su carácter.
   * @param character Carácter que representa el símbolo.
   */
  explicit Symbol(char character) : character_(character) {}

  /** @brief Destructor por defecto. */
  ~Symbol() = default;

  /** @brief Devuelve el carácter que representa el símbolo. */
  char GetCharacter() const { return character_; }

  /**
   * @brief Indica si este símbolo es el símbolo especial ε ('.').
   * @return true si es ε, false en caso contrario.
   */
  bool IsEpsilon() const { return character_ == kEpsilonCharacter; }

  /** @brief Representación del símbolo como cadena de un único carácter. */
  std::string ToString() const { return std::string(1, character_); }

  /** @brief Operador menor que, necesario para almacenar símbolos en
   * std::set/std::map. */
  bool operator<(const Symbol& other) const {
    return character_ < other.character_;
  }

  /** @brief Operador de igualdad entre símbolos. */
  bool operator==(const Symbol& other) const {
    return character_ == other.character_;
  }

  /** @brief Operador de desigualdad entre símbolos. */
  bool operator!=(const Symbol& other) const { return !(*this == other); }

  /** @brief Sobrecarga del operador de salida. */
  friend std::ostream& operator<<(std::ostream& output_stream,
                                  const Symbol& symbol) {
    output_stream << symbol.character_;
    return output_stream;
  }

 private:
  char character_; /**< Carácter que representa el símbolo. */
};

#endif  // SYMBOL_H_