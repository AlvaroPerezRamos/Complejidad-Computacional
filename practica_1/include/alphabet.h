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
 * @file alphabet.h
 * @brief Definición de la clase Alphabet: se usa tanto para Σ como para Γ.
 */

#ifndef ALPHABET_H_
#define ALPHABET_H_

#include <cstddef>
#include <iostream>
#include <set>
#include <string>

#include "symbol.h"

/**
 * @class Alphabet
 * @brief Representa un alfabeto finito de símbolos distintos (Σ o Γ).
 */
class Alphabet {
 public:
  /**
   * @brief Construye un alfabeto a partir de los símbolos de su línea del
   * fichero de configuración, separados por espacios en blanco (ej.
   * "a b" o "S A"). Se asume que la línea ya llega sin el comentario que
   * pueda seguir a un '#' ni las líneas en blanco.
   * @param symbols_line Símbolos del alfabeto, separados por espacios.
   * @throw MissingSectionError Si no se declara ningún símbolo.
   * @throw InvalidSymbolError Si algún símbolo tiene más de un carácter, o
   * es el carácter reservado '.'.
   * @throw DuplicatedElementError Si un símbolo aparece más de una vez.
   */
  explicit Alphabet(const std::string& symbols_line);

  /** @brief Destructor por defecto. */
  ~Alphabet() = default;

  /** @brief Devuelve el alfabeto como un conjunto de símbolos. */
  const std::set<Symbol>& GetAlphabet() const { return symbols_; }

  /** @brief Devuelve el número de símbolos del alfabeto. */
  std::size_t Size() const { return symbols_.size(); }

  /** @brief Indica si el alfabeto no contiene ningún símbolo. */
  bool IsEmpty() const { return symbols_.empty(); }

  /** @brief Comprueba si un símbolo pertenece al alfabeto. */
  bool Contains(const Symbol& symbol) const {
    return symbols_.count(symbol) > 0;
  }

  /**
   * @brief Inserta un símbolo en el alfabeto.
   * @param symbol Símbolo a insertar.
   * @return true si el símbolo no estaba ya presente y se ha insertado,
   * false si ya existía (y por tanto no se ha modificado el alfabeto).
   */
  bool InsertSymbol(const Symbol& symbol);

  /** @brief Operador de igualdad entre alfabetos. */
  bool operator==(const Alphabet& other) const {
    return symbols_ == other.symbols_;
  }

  /** @brief Operador de desigualdad entre alfabetos. */
  bool operator!=(const Alphabet& other) const { return !(*this == other); }

  /** @brief Sobrecarga del operador de salida. */
  friend std::ostream& operator<<(std::ostream& output_stream,
                                  const Alphabet& alphabet);

 private:
  /**
   * @brief Comprueba las invariantes de un token antes de insertarlo como
   * símbolo: exactamente un carácter, y distinto de '.'.
   * @param token Token leído de la línea del alfabeto.
   * @throw InvalidSymbolError Si se incumple alguna invariante.
   */
  void ValidateSymbolToken(const std::string& token) const;

  std::set<Symbol> symbols_; /**< Símbolos únicos del alfabeto. */
};

#endif  // ALPHABET_H_