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
 * Cada descripción que se reporta recibe también un ID propio (1, 2,
 * 3...), que Tracer apila en description_id_stack_ mientras dura su
 * exploración (empuja en ReportDescription(), retira en
 * ReportBacktracking()). Como esta pila sigue exactamente la recursión
 * de Simulator, cuando varios niveles retroceden uno detrás de otro sin
 * nada interesante en medio, no hace falta contarlos: basta con recordar
 * el ID del primero que se abandona y, al imprimir, decir a qué ID se ha
 * vuelto -que es el que queda en la cima de la pila tras retirar todos
 * los intermedios-. Una sola línea sirve igual para un retroceso que
 * para ocho, y además dice adónde se vuelve, no solo cuántos hay.
 * BeginChain() reinicia toda esta pila sin imprimir nada, por si quedara
 * algo pendiente de una cadena anterior abortada por una excepción (que
 * no llega a llamar a EndChain()).
 *
 * Historial de versiones
 *   23/09/2026 - Creación del fichero e implementación completa.
 *   23/09/2026 - Ampliación, el mismo día: los retrocesos consecutivos
 *                se agrupan en una sola línea ("<- retroceso xN").
 *   23/09/2026 - Rediseño, el mismo día: se sustituye el contador de
 *                retrocesos por un ID propio de cada descripción, así
 *                que la línea de retroceso agrupado dice a qué
 *                descripción concreta se vuelve, no cuántos pasos deshace.
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
   * @brief Informa de qué transición se aplica. Antes, vacía los
   * retrocesos pendientes. No hace nada si la traza está desactivada.
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
};

#endif  // TRACER_H_