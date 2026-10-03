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
 * @file tape_action.h
 * @brief Lo que una transición hace sobre UNA cinta concreta: símbolo
 * leído (para la clave), símbolo a escribir, y movimiento. Agrupar los
 * tres en una struct (en vez de tres vectores paralelos indexados por
 * número de cinta) hace imposible que se desincronicen entre sí.
 */

#ifndef TAPE_ACTION_H_
#define TAPE_ACTION_H_

#include "movement.h"
#include "symbol.h"

/** @brief Símbolo leído, símbolo a escribir y movimiento, para una cinta. */
struct TapeAction {
  Symbol read_symbol;
  Symbol write_symbol;
  Movement movement;

  bool operator==(const TapeAction& other) const {
    return read_symbol == other.read_symbol && write_symbol == other.write_symbol &&
           movement == other.movement;
  }
  bool operator!=(const TapeAction& other) const { return !(*this == other); }
};

#endif  // TAPE_ACTION_H_
