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
 * @file stack.cc
 * @brief Implementación de la clase Stack.
 *
 * Historial de versiones
 *   19/09/2026 - Creación e implementación completa.
 */

#include "../include/stack.h"
#include "../include/errors.h"

Symbol Stack::Top() const {
  if (IsEmpty()) {
    throw EmptyStackError("No se puede consultar la cima de una pila vacía.");
  }
  return Symbol(symbols_.front());
}

void Stack::Pop() {
  if (IsEmpty()) {
    throw EmptyStackError("No se puede desapilar de una pila vacía.");
  }
  symbols_.erase(0, 1);
}

void Stack::Push(const std::string& symbols_to_push) {
  symbols_ = symbols_to_push + symbols_;
}

/**
 * @brief Sobrecarga del operador de salida para la clase Stack.
 * @param output_stream Flujo de salida.
 * @param stack Pila a imprimir, de la cima al fondo.
 * @return El propio flujo de salida, para poder encadenar llamadas.
 */
std::ostream& operator<<(std::ostream& output_stream, const Stack& stack) {
  output_stream << stack.symbols_;
  return output_stream;
}