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
 * @file automaton_validator.h
 * @brief Definición de AutomatonValidator: los 5 avisos que necesitan el
 * PushdownAutomaton ya construido. Ninguno aborta el programa.
 */

#ifndef AUTOMATON_VALIDATOR_H_
#define AUTOMATON_VALIDATOR_H_

#include <ostream>
#include <set>

#include "pushdown_automaton.h"
#include "state.h"

/**
 * @class AutomatonValidator
 * @brief Comprueba las restricciones semánticas del enunciado que no
 * abortan la carga (avisos), y que necesitan el autómata ya construido.
 */
class AutomatonValidator {
 public:
  /** @brief Clase de utilidad: no se puede instanciar. */
  AutomatonValidator() = delete;

  /**
   * @brief Ejecuta las cinco comprobaciones de la sección 6 del
   * enunciado y escribe un aviso por cada una que se incumpla.
   * @param automaton Autómata ya construido y validado por
   * AutomatonParser (sin errores estructurales).
   * @param warnings_stream Flujo donde escribir los avisos ("[Aviso] ...").
   */
  static void Validate(const PushdownAutomaton& automaton,
                       std::ostream& warnings_stream);

 private:
  /** @brief Avisa si el autómata no tiene ninguna transición. */
  static void WarnIfNoTransitions(const PushdownAutomaton& automaton,
                                  std::ostream& warnings_stream);

  /** @brief Avisa de cada estado declarado que no está en reachable_states. */
  static void WarnUnreachableStates(const PushdownAutomaton& automaton,
                                    const std::set<State>& reachable_states,
                                    std::ostream& warnings_stream);

  /** @brief Avisa si ningún estado de F está en reachable_states. */
  static void WarnIfNoFinalStateReachable(
      const PushdownAutomaton& automaton,
      const std::set<State>& reachable_states, std::ostream& warnings_stream);

  /** @brief Avisa si Σ y Γ comparten algún símbolo. */
  static void WarnIfAlphabetsOverlap(const PushdownAutomaton& automaton,
                                     std::ostream& warnings_stream);

  /** @brief Avisa si no hay ninguna transición aplicable en (q0, Z0). */
  static void WarnIfNoTransitionAtStart(const PushdownAutomaton& automaton,
                                        std::ostream& warnings_stream);
};

#endif  // AUTOMATON_VALIDATOR_H_