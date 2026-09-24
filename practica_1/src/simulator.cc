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
 * @file simulator.cc
 * @brief Implementación de la clase Simulator.
 *
 * Historial de versiones
 *   23/09/2026 - Creación e implementación completa.
 *   23/09/2026 - Ampliación, el mismo día: integración de Tracer.
 */

#include "../include/simulator.h"

#include "../include/errors.h"
#include "../include/symbol.h"

Simulator::Simulator(const PushdownAutomaton& automaton,
                     std::ostream& trace_stream, bool trace_enabled)
    : automaton_(automaton), tracer_(trace_stream, trace_enabled) {
  tracer_.AssignTransitionNumbers(
      automaton_.GetTransitionFunction().GetOrderedTransitions(
          automaton_.GetStateDeclarationOrder()));
}

bool Simulator::Accepts(const Chain& chain) {
  explored_descriptions_count_ = 0;
  tracer_.BeginChain(chain);

  const InstantaneousDescription initial_description(
      automaton_.GetInitialState(), chain.GetText(),
      Stack(automaton_.GetInitialStackSymbol()));

  std::set<InstantaneousDescription> visited_descriptions;
  visited_descriptions.insert(initial_description);

  const bool accepted =
      ExploreDescription(initial_description, 0, visited_descriptions);
  tracer_.EndChain(accepted, explored_descriptions_count_);
  return accepted;
}

bool Simulator::ExploreDescription(
    const InstantaneousDescription& description, unsigned recursion_depth,
    std::set<InstantaneousDescription>& visited_descriptions) {
  if (description.IsInputConsumed() &&
      automaton_.IsFinalState(description.GetState())) {
    tracer_.ReportDescription(description, {});
    return true;
  }

  if (recursion_depth >= kMaxRecursionDepth) {
    throw SimulationLimitExceededError(
        "Se ha superado la profundidad máxima de "
        "recursión (" +
        std::to_string(kMaxRecursionDepth) + ").");
  }
  if (description.GetStack().Size() > kMaxStackSize) {
    throw SimulationLimitExceededError(
        "Se ha superado el tamaño máximo de la "
        "pila (" +
        std::to_string(kMaxStackSize) + " símbolos).");
  }
  ++explored_descriptions_count_;
  if (explored_descriptions_count_ > kMaxExploredDescriptions) {
    throw SimulationLimitExceededError(
        "Se ha superado el número máximo de "
        "descripciones instantáneas exploradas (" +
        std::to_string(kMaxExploredDescriptions) + ").");
  }

  const std::vector<Transition> applicable_transitions =
      GetApplicableTransitions(description);
  tracer_.ReportDescription(description, applicable_transitions);

  for (const Transition& transition : applicable_transitions) {
    const InstantaneousDescription next_description =
        ApplyTransition(description, transition);

    if (visited_descriptions.count(next_description) > 0) {
      continue;  // Ya visitada en esta rama: se poda para evitar ciclos.
    }

    tracer_.ReportAppliedTransition(transition);
    visited_descriptions.insert(next_description);
    if (ExploreDescription(next_description, recursion_depth + 1,
                           visited_descriptions)) {
      return true;
    }
    visited_descriptions.erase(
        next_description);  // Retroceso: libre para otras ramas.
    tracer_.ReportBacktracking();
  }

  return false;
}

std::vector<Transition> Simulator::GetApplicableTransitions(
    const InstantaneousDescription& description) const {
  if (description.GetStack().IsEmpty()) {
    return {};
  }

  const Symbol stack_top = description.GetStack().Top();
  const TransitionFunction& transition_function =
      automaton_.GetTransitionFunction();

  std::vector<Transition> applicable_transitions =
      transition_function.GetApplicableTransitions(
          description.GetState(), Symbol(kEpsilonCharacter), stack_top);

  if (!description.IsInputConsumed()) {
    const std::vector<Transition> transitions_with_input_symbol =
        transition_function.GetApplicableTransitions(
            description.GetState(), description.GetNextInputSymbol(),
            stack_top);
    applicable_transitions.insert(applicable_transitions.end(),
                                  transitions_with_input_symbol.begin(),
                                  transitions_with_input_symbol.end());
  }

  return applicable_transitions;
}

InstantaneousDescription Simulator::ApplyTransition(
    const InstantaneousDescription& description,
    const Transition& transition) const {
  std::string new_remaining_input = description.GetRemainingInput();
  if (!transition.IsEpsilonTransition()) {
    new_remaining_input.erase(0, 1);
  }

  Stack new_stack = description.GetStack();
  new_stack.Pop();
  new_stack.Push(transition.GetSymbolsToPush());

  return InstantaneousDescription(transition.GetDestinationState(),
                                  new_remaining_input, new_stack);
}