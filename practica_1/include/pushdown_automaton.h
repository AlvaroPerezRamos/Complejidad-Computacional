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
 * @file pushdown_automaton.h
 * @brief Definición de PushdownAutomaton. Estructura de datos inmutable;
 * el constructor no valida nada (lo hace AutomatonParser antes).
 */

#ifndef PUSHDOWN_AUTOMATON_H_
#define PUSHDOWN_AUTOMATON_H_

#include <set>
#include <vector>

#include "alphabet.h"
#include "state.h"
#include "symbol.h"
#include "transition_function.h"

/**
 * @class PushdownAutomaton
 * @brief Representa la séptupla (Q, Σ, Γ, δ, q0, Z0, F) de un autómata con
 * pila con finalización por estado final (APf).
 */
class PushdownAutomaton {
 public:
  /**
   * @brief Construye el autómata a partir de sus siete componentes, ya
   * validados por quien los construye.
   * @param state_declaration_order Estados en el orden en que se
   * declararon en la línea Q del fichero de configuración.
   * @param input_alphabet Alfabeto de entrada (Σ).
   * @param stack_alphabet Alfabeto de pila (Γ).
   * @param initial_state Estado inicial (q0).
   * @param initial_stack_symbol Símbolo inicial de la pila (Z0).
   * @param final_states Conjunto de estados finales (F).
   * @param transition_function Función de transición (δ) completa.
   */
  PushdownAutomaton(const std::vector<State>& state_declaration_order,
                    const Alphabet& input_alphabet,
                    const Alphabet& stack_alphabet, const State& initial_state,
                    const Symbol& initial_stack_symbol,
                    const std::vector<State>& final_states,
                    const TransitionFunction& transition_function);

  /** @brief Destructor por defecto. */
  ~PushdownAutomaton() = default;

  /** @brief Devuelve el conjunto de estados (Q). */
  const std::set<State>& GetStates() const { return states_; }

  /** @brief Devuelve los estados en el orden en que se declararon en el
   * fichero. */
  const std::vector<State>& GetStateDeclarationOrder() const {
    return state_declaration_order_;
  }

  /** @brief Devuelve el alfabeto de entrada (Σ). */
  const Alphabet& GetInputAlphabet() const { return input_alphabet_; }

  /** @brief Devuelve el alfabeto de pila (Γ). */
  const Alphabet& GetStackAlphabet() const { return stack_alphabet_; }

  /** @brief Devuelve el estado inicial (q0). */
  const State& GetInitialState() const { return initial_state_; }

  /** @brief Devuelve el símbolo inicial de la pila (Z0). */
  const Symbol& GetInitialStackSymbol() const { return initial_stack_symbol_; }

  /** @brief Devuelve el conjunto de estados finales (F). */
  const std::set<State>& GetFinalStates() const { return final_states_; }

  /** @brief Devuelve la función de transición (δ). */
  const TransitionFunction& GetTransitionFunction() const {
    return transition_function_;
  }

  /** @brief Indica si un estado pertenece a F. */
  bool IsFinalState(const State& state) const {
    return final_states_.count(state) > 0;
  }

  /**
   * @brief Calcula los estados alcanzables desde q0 siguiendo las
   * transiciones de δ (sin tener en cuenta la pila ni la entrada, solo la
   * estructura del grafo de estados). Lo usará AutomatonValidator para el
   * aviso de "estado inalcanzable desde q0".
   * @return El conjunto de estados alcanzables, incluyendo siempre q0.
   */
  std::set<State> ComputeReachableStates() const;

 private:
  std::set<State> states_;                     /**< Conjunto de estados (Q). */
  std::vector<State> state_declaration_order_; /**< Estados en el orden de
                                                  declaración en el fichero. */
  Alphabet input_alphabet_;                    /**< Alfabeto de entrada (Σ). */
  Alphabet stack_alphabet_;                    /**< Alfabeto de pila (Γ). */
  State initial_state_;                        /**< Estado inicial (q0). */
  Symbol initial_stack_symbol_;  /**< Símbolo inicial de la pila (Z0). */
  std::set<State> final_states_; /**< Conjunto de estados finales (F). */
  TransitionFunction transition_function_; /**< Función de transición (δ). */
};

#endif  // PUSHDOWN_AUTOMATON_H_