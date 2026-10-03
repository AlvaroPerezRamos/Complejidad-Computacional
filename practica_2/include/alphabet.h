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
 * @file alphabet.h
 * @brief Definición de la clase Alphabet: se usa tanto para Σ como para Γ
 * (un único Γ compartido por todas las cintas). A diferencia de P01, no
 * prohíbe ningún carácter por sí misma: la restricción de que el blanco
 * no pertenezca a Σ se comprueba aparte, al leer la línea del blanco
 * (TuringMachineParser), no aquí.
 */

#ifndef ALPHABET_H_
#define ALPHABET_H_

#include <cstddef>
#include <iostream>
#include <set>
#include <string>

#include "symbol.h"

/** @brief Alfabeto finito de símbolos distintos (Σ o Γ). */
class Alphabet {
 public:
  /**
   * @brief Construye el alfabeto a partir de su línea del fichero de
   * configuración (símbolos separados por espacios, ej. "a b").
   * @throw MissingSectionError Si no se declara ningún símbolo.
   * @throw InvalidSymbolError Si algún símbolo no tiene un único carácter.
   * @throw DuplicatedElementError Si un símbolo aparece repetido.
   */
  explicit Alphabet(const std::string& symbols_line);

  ~Alphabet() = default;

  const std::set<Symbol>& GetAlphabet() const { return symbols_; }
  std::size_t Size() const { return symbols_.size(); }
  bool IsEmpty() const { return symbols_.empty(); }
  bool Contains(const Symbol& symbol) const { return symbols_.count(symbol) > 0; }

  /** @return true si el símbolo era nuevo; false si ya estaba presente. */
  bool InsertSymbol(const Symbol& symbol);

  bool operator==(const Alphabet& other) const { return symbols_ == other.symbols_; }
  bool operator!=(const Alphabet& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& output_stream, const Alphabet& alphabet);

 private:
  /** @throw InvalidSymbolError Si el token no tiene un único carácter. */
  void ValidateSymbolToken(const std::string& token) const;

  std::set<Symbol> symbols_;
};

#endif  // ALPHABET_H_
