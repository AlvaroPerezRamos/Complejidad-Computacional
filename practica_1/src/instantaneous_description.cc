/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 23/09/2026
 * @file instantaneous_description.cc
 * @brief Implementación de la clase InstantaneousDescription.
 *
 * Historial de versiones
 *   23/09/2026 - Creación e implementación completa.
 *   23/09/2026 - Implementación del método GetNextInputSymbol.
 */

#include "../include/instantaneous_description.h"

#include "../include/errors.h"

Symbol InstantaneousDescription::GetNextInputSymbol() const {
  if (IsInputConsumed()) {
    throw SimulationError(
        "No se puede consultar el siguiente símbolo de "
        "entrada: la entrada ya está consumida.");
  }
  return Symbol(remaining_input_.front());
}