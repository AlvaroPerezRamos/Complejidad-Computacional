/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 19/09/2026
 * @file transition.cc
 * @brief Implementación de la clase Transition.
 */

#include "../include/transition.h"

namespace {

const std::string kDeltaSymbol = "\u03B4";      // δ
const std::string kElementOfSymbol = "\u220B";  // ∋

}  // namespace

std::string Transition::ToString() const {
  const std::string pushed_symbols = symbols_to_push_.empty()
                                         ? std::string(1, kEpsilonCharacter)
                                         : symbols_to_push_;
  return kDeltaSymbol + "(" + origin_state_.GetName() + ", " +
         input_symbol_.ToString() + ", " + stack_symbol_.ToString() + ") " +
         kElementOfSymbol + " (" + destination_state_.GetName() + ", " +
         pushed_symbols + ")";
}

/**
 * @brief Sobrecarga del operador de salida para la clase Transition.
 * @param output_stream Flujo de salida.
 * @param transition Transición a imprimir.
 * @return El propio flujo de salida, para poder encadenar llamadas.
 */
std::ostream& operator<<(std::ostream& output_stream,
                         const Transition& transition) {
  output_stream << transition.ToString();
  return output_stream;
}