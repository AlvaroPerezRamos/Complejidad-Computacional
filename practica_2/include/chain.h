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
 * @file chain.h
 * @brief Definición de la clase Chain. La cadena vacía se escribe como el
 * símbolo blanco de la MT (p. ej. "."), o como texto vacío. No es
 * ambiguo: el blanco nunca pertenece a Σ (lo garantiza el parser).
 */

#ifndef CHAIN_H_
#define CHAIN_H_

#include <iostream>
#include <string>

#include "alphabet.h"

/** @brief Cadena de entrada, validada contra Σ al construirse. */
class Chain {
 public:
  /**
   * @brief Construye la cadena. Un texto vacío, o formado solo por
   * blank_symbol, es la cadena vacía.
   * @throw ChainError Si algún símbolo no pertenece a input_alphabet.
   */
  Chain(const std::string& text, const Alphabet& input_alphabet,
        const Symbol& blank_symbol);

  const std::string& GetText() const { return text_; }
  bool IsEmpty() const { return text_.empty(); }
  std::size_t Length() const { return text_.length(); }

  friend std::ostream& operator<<(std::ostream& output_stream,
                                  const Chain& chain);

 private:
  /** @throw ChainError Si algún símbolo de raw_text no pertenece a
   * input_alphabet. */
  static void ValidateAgainstAlphabet(const std::string& raw_text,
                                      const Alphabet& input_alphabet);

  std::string text_;
};

#endif  // CHAIN_H_