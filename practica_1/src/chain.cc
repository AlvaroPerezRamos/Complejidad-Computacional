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
 * @file chain.cc
 * @brief Implementación de la clase Chain.
 */

#include "../include/chain.h"
#include "../include/errors.h"

Chain::Chain(const std::string& text, const Alphabet& input_alphabet) {
  const bool represents_epsilon =
      text.empty() || text == std::string(1, kEpsilonCharacter);
  if (represents_epsilon) {
    text_ = "";
    return;
  }
  ValidateAgainstAlphabet(text, input_alphabet);
  text_ = text;
}

void Chain::ValidateAgainstAlphabet(const std::string& raw_text,
                                    const Alphabet& input_alphabet) {
  for (char character : raw_text) {
    if (!input_alphabet.Contains(Symbol(character))) {
      throw ChainError("La cadena '" + raw_text + "' contiene el símbolo '" +
                       std::string(1, character) +
                       "', que no pertenece al alfabeto de entrada.");
    }
  }
}

/**
 * @brief Sobrecarga del operador de salida para la clase Chain.
 * @param output_stream Flujo de salida.
 * @param chain Cadena a imprimir.
 * @return El propio flujo de salida, para poder encadenar llamadas.
 */
std::ostream& operator<<(std::ostream& output_stream, const Chain& chain) {
  output_stream << (chain.text_.empty() ? std::string(1, kEpsilonCharacter)
                                        : chain.text_);
  return output_stream;
}