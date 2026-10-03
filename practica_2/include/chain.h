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
 * @brief Definición de la clase Chain. Sin convenio de ε: no hace falta
 * ("." no tiene ningún significado especial), una cadena vacía ya se
 * representa directamente como texto vacío.
 */

#ifndef CHAIN_H_
#define CHAIN_H_

#include <iostream>
#include <string>

#include "alphabet.h"

/** @brief Cadena de entrada, validada contra Σ al construirse. */
class Chain {
 public:
  /** @throw ChainError Si algún símbolo no pertenece a input_alphabet. */
  Chain(const std::string& text, const Alphabet& input_alphabet);

  const std::string& GetText() const { return text_; }
  bool IsEmpty() const { return text_.empty(); }
  std::size_t Length() const { return text_.length(); }

  friend std::ostream& operator<<(std::ostream& output_stream, const Chain& chain);

 private:
  /** @throw ChainError Si algún símbolo de raw_text no pertenece a input_alphabet. */
  static void ValidateAgainstAlphabet(const std::string& raw_text,
                                      const Alphabet& input_alphabet);

  std::string text_;
};

#endif  // CHAIN_H_
