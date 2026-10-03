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
 * @file turing_configuration.h
 * @brief Definición de TuringConfiguration: estado actual + las N cintas.
 * A diferencia de InstantaneousDescription (P01), es deliberadamente
 * MUTABLE: al ser determinista no hay backtracking ni ramas que
 * coexistan, así que no hace falta inmutabilidad ni operator==/operator<
 * para detectar ciclos -se aplica la transición "in place".
 */

#ifndef TURING_CONFIGURATION_H_
#define TURING_CONFIGURATION_H_

#include <utility>
#include <vector>

#include "state.h"
#include "tape.h"
#include "transition.h"

class TuringConfiguration {
 public:
  TuringConfiguration(const State& initial_state, std::vector<Tape> tapes)
      : current_state_(initial_state), tapes_(std::move(tapes)) {}

  const State& GetCurrentState() const { return current_state_; }
  const std::vector<Tape>& GetTapes() const { return tapes_; }

  /** @brief El símbolo bajo el cabezal de cada cinta, en orden. */
  std::vector<Symbol> ReadSymbols() const;

  /** @brief Escribe y mueve cada cinta según transition, y cambia de estado. */
  void ApplyTransition(const Transition& transition);

 private:
  State current_state_;
  std::vector<Tape> tapes_;
};

#endif  // TURING_CONFIGURATION_H_
