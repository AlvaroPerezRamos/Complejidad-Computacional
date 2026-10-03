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
 * @file transition_function.h
 * @brief Definición de TransitionFunction (la función δ completa).
 * Determinista: cada clave mapea a una única Transition, a diferencia de
 * TransitionFunction de P01 (vector<Transition> por clave).
 */

#ifndef TRANSITION_FUNCTION_H_
#define TRANSITION_FUNCTION_H_

#include <cstddef>
#include <map>
#include <optional>
#include <vector>

#include "state.h"
#include "transition.h"

class TransitionFunction {
 public:
  TransitionFunction() = default;

  /**
   * @return true si es una transición nueva; false si ya existía
   * exactamente igual (aviso: se ignora, no error).
   * @throw NonDeterministicTransitionError Si la clave ya existía con un
   * destino, escritura o movimiento distintos.
   */
  bool Insert(const Transition& transition);

  /** @brief La transición aplicable desde (origin_state, read_symbols), si existe. */
  std::optional<Transition> GetTransition(const State& origin_state,
                                          const std::vector<Symbol>& read_symbols) const;

  /** @brief Transiciones desde origin_state (para ComputeReachableStates). */
  std::vector<Transition> GetTransitionsFrom(const State& origin_state) const;

  std::size_t Size() const { return transitions_by_key_.size(); }

 private:
  std::map<TransitionKey, Transition> transitions_by_key_;
};

#endif  // TRANSITION_FUNCTION_H_
