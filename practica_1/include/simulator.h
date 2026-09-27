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
 * @file simulator.h
 * @brief Definición de Simulator: búsqueda en profundidad con retroceso
 * sobre el árbol de descripciones instantáneas (algoritmo y salvaguardas
 * en AutomataPila.md, sección 5). Construye internamente su Tracer.
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <cstddef>
#include <ostream>
#include <set>
#include <vector>

#include "chain.h"
#include "instantaneous_description.h"
#include "pushdown_automaton.h"
#include "tracer.h"
#include "transition.h"

/**
 * @class Simulator
 * @brief Determina si un PushdownAutomaton acepta una cadena, explorando
 * su árbol de descripciones instantáneas con retroceso.
 */
class Simulator {
 public:
  /**
   * @brief Construye el simulador para un autómata concreto.
   * @param automaton Autómata ya construido y validado (por
   * AutomatonParser).
   * @param trace_stream Flujo donde escribir la traza (pantalla o
   * fichero).
   * @param trace_enabled Si la traza está activada (-trace y/n).
   */
  Simulator(const PushdownAutomaton& automaton, std::ostream& trace_stream,
            bool trace_enabled);

  /** @brief Destructor por defecto. */
  ~Simulator() = default;

  /**
   * @brief Determina si el autómata acepta una cadena de entrada.
   * @param chain Cadena a comprobar, ya validada contra Σ.
   * @return true si existe alguna secuencia de transiciones que consuma
   * la cadena por completo y alcance un estado de F; false si se agotan
   * todas las ramas sin conseguirlo.
   * @throw SimulationLimitExceededError Si se supera alguna de las
   * salvaguardas de la sección 5. Según la sección 6 del enunciado, quien
   * llame a Accepts() debe descartar esta cadena y continuar con la
   * siguiente, no abortar el programa.
   */
  bool Accepts(const Chain& chain);

 private:
  /** @brief Tamaño máximo de la pila antes de abortar la rama (sección 5). */
  static constexpr std::size_t kMaxStackSize = 1000;

  /** @brief Número máximo de descripciones instantáneas exploradas por cadena
   * (sección 5). */
  static constexpr unsigned long kMaxExploredDescriptions = 200000;

  /** @brief Profundidad máxima de la recursión antes de abortar (sección 5). */
  static constexpr unsigned kMaxRecursionDepth = 2000;

  /**
   * @brief Explora, con retroceso, el árbol de descripciones instantáneas
   * a partir de description.
   * @param description Descripción instantánea actual.
   * @param recursion_depth Profundidad de description en el árbol de
   * exploración (0 para la descripción inicial).
   * @param visited_descriptions Descripciones ya visitadas en la rama
   * actual (no en todo el árbol): se inserta al bajar y se retira al
   * retroceder, para no confundir "ya visitada en esta rama" (un ciclo
   * real) con "ya visitada en otra rama" (que no es ningún problema).
   * @return true si description (o alguna alcanzable desde ella) resulta
   * en aceptación.
   * @throw SimulationLimitExceededError Si se supera alguna salvaguarda.
   */
  bool ExploreDescription(
      const InstantaneousDescription& description, unsigned recursion_depth,
      std::set<InstantaneousDescription>& visited_descriptions);

  /**
   * @brief Calcula T = δ(q, a, X) ∪ δ(q, ε, X), donde (q, w, X·α) es
   * description (a es el primer símbolo de w, si lo hay; X es la cima de
   * la pila). Si la pila está vacía no hay cima que consultar, así que no
   * hay ninguna transición aplicable.
   */
  std::vector<Transition> GetApplicableTransitions(
      const InstantaneousDescription& description) const;

  /**
   * @brief Aplica una transición a una descripción instantánea y devuelve
   * la descripción resultante, sin modificar description (es inmutable).
   * @param description Descripción de partida.
   * @param transition Transición a aplicar (debe ser una de las que
   * devuelve GetApplicableTransitions(description); este método no lo
   * comprueba).
   */
  InstantaneousDescription ApplyTransition(
      const InstantaneousDescription& description,
      const Transition& transition) const;

  PushdownAutomaton automaton_; /**< Autómata que se está simulando. */
  Tracer tracer_; /**< Traza de la exploración (no-op si está desactivada). */
  unsigned long explored_descriptions_count_ =
      0; /**< Descripciones exploradas en la cadena actual. */
};

#endif  // SIMULATOR_H_