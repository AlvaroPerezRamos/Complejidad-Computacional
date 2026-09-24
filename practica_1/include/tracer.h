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
 * @file tracer.h
 * @brief Definición de la clase Tracer.
 *
 * Formatea y encamina la traza (a pantalla o a fichero, según el
 * ostream& que reciba: la elección entre uno u otro es cosa de quien
 * construya el Tracer, no suya). Cada fila muestra los cuatro campos que
 * se acordaron -estado, cadena pendiente, pila y transiciones
 * aplicables- en vez de las columnas ID/Profundidad del ejemplo original
 * del README, que se descartaron explícitamente por no pedirse.
 *
 * is_enabled_ (el equivalente a -trace y/n) hace que todos los métodos de
 * reporte sean no-op cuando es false: así Simulator puede llamarlos
 * siempre, sin comprobar el modo en cada paso.
 *
 * AssignTransitionNumbers() debe llamarse una vez, con
 * TransitionFunction::GetOrderedTransitions() ya calculado, antes de
 * comprobar ninguna cadena: es lo que permite que ReportDescription() y
 * ReportAppliedTransition() se refieran a cada transición por su número
 * (1..N, en el orden que ya se resolvió: por estado de origen, según su
 * declaración en Q, autobucles antes que el resto) en vez de repetir su
 * forma completa en cada fila.
 *
 * Historial de versiones
 *   23/09/2026 - Creación del fichero e implementación completa.
 */

#ifndef TRACER_H_
#define TRACER_H_

#include <cstddef>
#include <map>
#include <ostream>
#include <vector>

#include "chain.h"
#include "instantaneous_description.h"
#include "transition.h"

/**
 * @class Tracer
 * @brief Formatea y escribe la traza de la exploración de Simulator.
 */
class Tracer {
 public:
  /**
   * @brief Construye el Tracer.
   * @param output_stream Flujo donde escribir la traza (pantalla o
   * fichero; a este Tracer le da igual cuál sea).
   * @param is_enabled Si la traza está activada (-trace y/n).
   */
  Tracer(std::ostream& output_stream, bool is_enabled)
      : output_stream_(output_stream), is_enabled_(is_enabled) {}

  /** @brief Destructor por defecto. */
  ~Tracer() = default;

  /**
   * @brief Asigna a cada transición su número (1..N) según el orden ya
   * calculado por TransitionFunction::GetOrderedTransitions(). Debe
   * llamarse una vez, antes de comprobar ninguna cadena.
   */
  void AssignTransitionNumbers(
      const std::vector<Transition>& ordered_transitions);

  /** @brief Marca el comienzo de la traza de una cadena. No hace nada si la
   * traza está desactivada. */
  void BeginChain(const Chain& chain);

  /**
   * @brief Informa de una descripción instantánea y de las transiciones
   * aplicables desde ella (por su número). No hace nada si la traza está
   * desactivada.
   */
  void ReportDescription(const InstantaneousDescription& description,
                         const std::vector<Transition>& applicable_transitions);

  /** @brief Informa de qué transición se aplica. No hace nada si la traza está
   * desactivada. */
  void ReportAppliedTransition(const Transition& transition);

  /** @brief Informa de un retroceso. No hace nada si la traza está desactivada.
   */
  void ReportBacktracking();

  /**
   * @brief Marca el final de la traza de una cadena, con el veredicto y
   * el número de descripciones exploradas. No hace nada si la traza está
   * desactivada.
   */
  void EndChain(bool accepted, unsigned long explored_descriptions);

 private:
  /**
   * @brief Devuelve el número asignado a una transición.
   * @return El número asignado, o 0 si AssignTransitionNumbers() no se
   * ha llamado todavía o la transición no estaba entre las asignadas (no
   * debería ocurrir en un uso correcto).
   */
  std::size_t GetTransitionNumber(const Transition& transition) const;

  std::ostream& output_stream_; /**< Flujo donde escribir la traza. */
  bool is_enabled_;             /**< Si la traza está activada. */
  std::map<Transition, std::size_t>
      transition_numbers_; /**< Número asignado a cada transición. */
};

#endif  // TRACER_H_