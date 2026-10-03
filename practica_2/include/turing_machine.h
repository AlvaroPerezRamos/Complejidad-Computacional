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
 * @file turing_machine.h
 * @brief Definición de TuringMachine: la séptupla (Q, Σ, Γ, s, b, F, δ) +
 * el número de cintas. Estructura de datos inmutable; el constructor no
 * valida nada (lo hace TuringMachineParser antes).
 */

#ifndef TURING_MACHINE_H_
#define TURING_MACHINE_H_

#include <set>
#include <vector>

#include "alphabet.h"
#include "state.h"
#include "symbol.h"
#include "transition_function.h"

class TuringMachine {
 public:
  TuringMachine(const std::vector<State>& state_declaration_order,
                const Alphabet& input_alphabet, const Alphabet& tape_alphabet,
                const State& initial_state, const Symbol& blank_symbol,
                const std::vector<State>& final_states, int tape_count,
                const TransitionFunction& transition_function);

  const std::set<State>& GetStates() const { return states_; }
  const std::vector<State>& GetStateDeclarationOrder() const { return state_declaration_order_; }
  const Alphabet& GetInputAlphabet() const { return input_alphabet_; }
  const Alphabet& GetTapeAlphabet() const { return tape_alphabet_; }
  const State& GetInitialState() const { return initial_state_; }
  const Symbol& GetBlankSymbol() const { return blank_symbol_; }
  const std::set<State>& GetFinalStates() const { return final_states_; }
  int GetTapeCount() const { return tape_count_; }
  const TransitionFunction& GetTransitionFunction() const { return transition_function_; }

  bool IsFinalState(const State& state) const { return final_states_.count(state) > 0; }

  /** @brief Estados alcanzables desde q0 siguiendo δ. Incluye siempre q0. */
  std::set<State> ComputeReachableStates() const;

 private:
  std::set<State> states_;
  std::vector<State> state_declaration_order_;
  Alphabet input_alphabet_;
  Alphabet tape_alphabet_;
  State initial_state_;
  Symbol blank_symbol_;
  std::set<State> final_states_;
  int tape_count_;
  TransitionFunction transition_function_;
};

#endif  // TURING_MACHINE_H_
