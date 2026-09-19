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
 * @file chain.h
 * @brief Definición de la clase Chain
 *
 * Historial de versiones
 *   18/09/2026 - Creación del fichero y definición de la clase Chain.
 *   19/09/2026 - Creación (primera versión) del código.
 */

#ifndef CHAIN_H_
#define CHAIN_H_

#include <iostream>
#include <string>

#include "../include/alphabet.h"

/**
 * @class Chain
 * @brief Representa una cadena de entrada, validada contra el alfabeto de
 * entrada (Σ) del autómata en el momento de construirse.
 */
class Chain {
 public:
  /**
   * @brief Construye una cadena de entrada.
   * @param text Texto tal y como se ha leído, por teclado o de fichero. El
   * valor "." (o una cadena vacía) se interpreta como ε.
   * @param input_alphabet Alfabeto de entrada (Σ) contra el que se valida.
   * @throw ChainError Si algún símbolo de la cadena no pertenece a Σ.
   */
  Chain(const std::string& text, const Alphabet& input_alphabet);

  /** @brief Devuelve el texto de la cadena, ya normalizado (vacío si es ε). */
  const std::string& GetText() const { return text_; }

  /** @brief Indica si la cadena es la cadena vacía (ε). */
  bool IsEmpty() const { return text_.empty(); }

  /** @brief Devuelve la longitud de la cadena. */
  std::size_t Length() const { return text_.length(); }

  /** @brief Sobrecarga del operador de salida (imprime '.' si la cadena es ε).
   */
  friend std::ostream& operator<<(std::ostream& output_stream,
                                  const Chain& chain);

 private:
  /**
   * @brief Comprueba que todos los símbolos de raw_text pertenecen al
   * alfabeto de entrada.
   * @param raw_text Texto a validar.
   * @param input_alphabet Alfabeto de entrada (Σ).
   * @throw ChainError Si encuentra algún símbolo ajeno a Σ.
   */
  static void ValidateAgainstAlphabet(const std::string& raw_text,
                                      const Alphabet& input_alphabet);

  std::string text_; /**< Texto de la cadena, normalizado (vacío si es ε). */
};
#endif  // CHAIN_H_