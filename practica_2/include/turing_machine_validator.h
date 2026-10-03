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
 * @file turing_machine_validator.h
 * @brief Definición de TuringMachineValidator: avisos que necesitan la
 * TuringMachine ya construida. No los exige el enunciado; se incluyen
 * porque cualquier información de más se consideró bienvenida. Ninguno
 * aborta el programa.
 *
 * A diferencia de AutomatonValidator (P01), NO comprueba "Σ ∩ Γ ≠ ∅": en
 * una MT, Σ ⊆ Γ es lo normal (la cinta debe poder representar la propia
 * entrada), no una coincidencia sospechosa como en un AP. Mantener ese
 * aviso aquí dispararía en cualquier MT bien diseñada.
 */

#ifndef TURING_MACHINE_VALIDATOR_H_
#define TURING_MACHINE_VALIDATOR_H_

#include <ostream>
#include <set>

#include "state.h"
#include "turing_machine.h"

/** @brief Clase de utilidad sin estado (constructor borrado, métodos estáticos). */
class TuringMachineValidator {
 public:
  TuringMachineValidator() = delete;

  /** @brief Ejecuta las comprobaciones y escribe un "[Aviso] ..." por cada una que se incumpla. */
  static void Validate(const TuringMachine& machine, std::ostream& warnings_stream);

 private:
  static void WarnIfNoTransitions(const TuringMachine& machine, std::ostream& warnings_stream);
  static void WarnUnreachableStates(const TuringMachine& machine,
                                    const std::set<State>& reachable_states,
                                    std::ostream& warnings_stream);
  static void WarnIfNoFinalStateReachable(const TuringMachine& machine,
                                          const std::set<State>& reachable_states,
                                          std::ostream& warnings_stream);
};

#endif  // TURING_MACHINE_VALIDATOR_H_
