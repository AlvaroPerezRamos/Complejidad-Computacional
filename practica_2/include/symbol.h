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
 * @file symbol.h
 * @brief Definición de la clase Symbol. Sin convenio de ε (no existe en
 * una MT): el blanco es un símbolo normal de Γ, sin tratamiento especial
 * a nivel de esta clase.
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <iostream>
#include <string>

/** @brief Símbolo de un alfabeto (Σ o Γ). */
class Symbol {
 public:
  explicit Symbol(char character) : character_(character) {}
  ~Symbol() = default;

  /** @brief Devuelve el carácter que representa el símbolo. */
  char GetCharacter() const { return character_; }

  /** @brief Representación como cadena de un carácter. */
  std::string ToString() const { return std::string(1, character_); }

  bool operator<(const Symbol& other) const { return character_ < other.character_; }
  bool operator==(const Symbol& other) const { return character_ == other.character_; }
  bool operator!=(const Symbol& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& output_stream, const Symbol& symbol) {
    output_stream << symbol.character_;
    return output_stream;
  }

 private:
  char character_;
};

#endif  // SYMBOL_H_
