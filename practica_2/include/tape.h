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
 * @file tape.h
 * @brief Definición de Tape: cinta infinita bidireccional.
 *
 * Se guarda como map<posición, símbolo> disperso (solo las celdas
 * distintas del blanco ocupan memoria) en vez de un vector con
 * desplazamiento: así la bidireccionalidad es gratis, el índice admite
 * valores negativos sin ningún caso especial de "extremo".
 */

#ifndef TAPE_H_
#define TAPE_H_

#include <map>
#include <string>

#include "movement.h"
#include "symbol.h"

/** @brief Cinta infinita bidireccional de una Máquina de Turing. */
class Tape {
 public:
  /** @brief Cinta completamente en blanco, cabezal en la posición 0. */
  explicit Tape(const Symbol& blank_symbol) : blank_symbol_(blank_symbol) {}

  /**
   * @brief Cinta con initial_content escrito a partir de la posición 0; el
   * cabezal vuelve a la posición 0 al terminar (el primer símbolo de la
   * entrada, tal y como exige el enunciado).
   */
  Tape(const Symbol& blank_symbol, const std::string& initial_content);

  /** @brief Símbolo bajo el cabezal (blank_symbol_ si la celda no se ha escrito). */
  Symbol Read() const { return SymbolAt(head_position_); }

  /** @brief Escribe en la celda bajo el cabezal (no mueve el cabezal). */
  void Write(const Symbol& symbol);

  void Move(Movement movement);

  long long GetHeadPosition() const { return head_position_; }

  /** @brief P. ej. "ab[c]d" si el cabezal apunta a 'c'. */
  std::string ToString() const;

 private:
  Symbol SymbolAt(long long position) const;

  Symbol blank_symbol_;
  std::map<long long, Symbol> cells_;
  long long head_position_ = 0;
};

#endif  // TAPE_H_
