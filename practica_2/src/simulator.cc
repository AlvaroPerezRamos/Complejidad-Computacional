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
 * @file simulator.cc
 * @brief Implementación de la clase Simulator.
 */

#include "../include/simulator.h"

#include <optional>
#include <utility>
#include <vector>

#include "../include/turing_configuration.h"

TuringRunResult Simulator::Run(const Chain& chain) const {
  std::vector<Tape> tapes;
  tapes.emplace_back(machine_.GetBlankSymbol(), chain.GetText());
  for (int tape_index = 1; tape_index < machine_.GetTapeCount(); ++tape_index) {
    tapes.emplace_back(machine_.GetBlankSymbol());
  }

  TuringConfiguration configuration(machine_.GetInitialState(), std::move(tapes));

  while (true) {
    const std::optional<Transition> transition = machine_.GetTransitionFunction().GetTransition(
        configuration.GetCurrentState(), configuration.ReadSymbols());
    if (!transition.has_value()) {
      break;  // Ninguna transición aplicable: la MT se detiene (criterio del enunciado).
    }
    configuration.ApplyTransition(*transition);
  }

  return TuringRunResult{machine_.IsFinalState(configuration.GetCurrentState()),
                         configuration.GetTapes()[0].ToString()};
}
