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
 * @brief Definición de la clase AutomatonValidator.
 *
 * Comprueba exactamente los cinco avisos de la sección 6 del enunciado
 * que necesitan el PushdownAutomaton ya construido -no antes, porque
 * requieren el grafo de transiciones completo-: autómata sin
 * transiciones, estados inalcanzables desde q0, ningún estado de F
 * alcanzable, Σ ∩ Γ ≠ ∅, y ninguna transición aplicable en (q0, Z0).
 * Ninguno de los cinco aborta el programa: solo informan.
 *
 * Deliberadamente NO comprueba, porque el enunciado no lo pide, si un
 * estado alcanzable desde q0 puede a su vez llegar a algún estado de F
 * (alcanzabilidad hacia atrás desde F, el "estado muerto" o "trampa" en
 * el sentido clásico de minimización de autómatas). Es un chequeo más
 * fino que el que pide la tabla de avisos.
 *
 * Es una clase de utilidad sin estado: todos sus métodos son estáticos y
 * su constructor está borrado para impedir instanciarla.
 *
 * Historial de versiones
 *   19/09/2026 - Creación del fichero e implementación completa.
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