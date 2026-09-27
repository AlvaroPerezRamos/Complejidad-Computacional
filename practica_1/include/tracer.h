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
 * @brief Definición de Tracer: formatea y escribe la traza de Simulator
 * (a pantalla o fichero, según el ostream& recibido). Ver AutomataPila.md
 * para el formato completo (IDs de descripción, agrupación de retrocesos).
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

  /**
   * @brief Marca el comienzo de la traza de una cadena. Reinicia la
   * numeración de descripciones y descarta cualquier retroceso pendiente
   * de una cadena anterior. No hace nada si la traza está desactivada.
   */
  void BeginChain(const Chain& chain);

  /**
   * @brief Informa de una descripción instantánea (asignándole su ID) y
   * de las transiciones aplicables desde ella (por su número). Antes,
   * vacía los retrocesos pendientes. No hace nada si la traza está
   * desactivada.
   */
  void ReportDescription(const InstantaneousDescription& description,
                         const std::vector<Transition>& applicable_transitions);

  /**
   * @brief Informa de qué transición se aplica, salvo que fuera la única
   * aplicable en la última descripción reportada (no había elección real
   * que anunciar). Antes, si va a imprimir, vacía los retrocesos
   * pendientes. No hace nada si la traza está desactivada.
   */
  void ReportAppliedTransition(const Transition& transition);

  /**
   * @brief Retira de la pila el ID de la descripción que se abandona.
   * No imprime nada todavía: se acumula hasta que otra llamada distinta
   * lo vacíe (ver la explicación en la cabecera del fichero). No hace
   * nada si la traza está desactivada.
   */
  void ReportBacktracking();

  /**
   * @brief Marca el final de la traza de una cadena, con el veredicto y
   * el número de descripciones exploradas. Antes, vacía los retrocesos
   * pendientes. No hace nada si la traza está desactivada.
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

  /**
   * @brief Si hay algún retroceso acumulado sin imprimir, escribe la
   * línea ("desde la descripción X hasta la Y") y limpia el estado
   * pendiente. Si no hay ninguno, no hace nada.
   */
  void FlushPendingBacktracks();

  std::ostream& output_stream_; /**< Flujo donde escribir la traza. */
  bool is_enabled_;             /**< Si la traza está activada. */
  std::map<Transition, std::size_t>
      transition_numbers_; /**< Número asignado a cada transición. */
  std::vector<unsigned long>
      description_id_stack_; /**< IDs de la rama actual (sigue la recursión). */
  unsigned long next_description_id_ = 1; /**< Próximo ID a asignar. */
  bool has_pending_backtrack_ =
      false; /**< Si hay retrocesos sin imprimir todavía. */
  unsigned long pending_backtrack_from_id_ =
      0; /**< ID de la primera descripción abandonada en la racha actual. */
  std::size_t last_applicable_transitions_count_ =
      0; /**< Cuántas transiciones tenía la última descripción reportada. */
};

#endif  // TRACER_H_