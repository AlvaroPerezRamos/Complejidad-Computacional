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
 * @file tracer.cc
 * @brief Implementación de la clase Tracer.
 *
 * Historial de versiones
 *   23/09/2026 - Creación e implementación completa.
 */

#include "../include/tracer.h"

#include <string>

namespace {
constexpr int kSeparatorWidth = 80;
}  // namespace

void Tracer::AssignTransitionNumbers(
    const std::vector<Transition>& ordered_transitions) {
  transition_numbers_.clear();
  std::size_t number = 1;
  for (const Transition& transition : ordered_transitions) {
    transition_numbers_[transition] = number++;
  }
}

std::size_t Tracer::GetTransitionNumber(const Transition& transition) const {
  const auto found_entry = transition_numbers_.find(transition);
  return found_entry == transition_numbers_.end() ? 0 : found_entry->second;
}

void Tracer::BeginChain(const Chain& chain) {
  if (!is_enabled_) return;
  output_stream_ << std::string(kSeparatorWidth, '=') << "\n";
  output_stream_ << " Traza del reconocimiento de la cadena: " << chain << "\n";
  output_stream_ << std::string(kSeparatorWidth, '=') << "\n";
}

void Tracer::ReportDescription(
    const InstantaneousDescription& description,
    const std::vector<Transition>& applicable_transitions) {
  if (!is_enabled_) return;

  const std::string remaining_input = description.GetRemainingInput().empty()
                                          ? "ε"
                                          : description.GetRemainingInput();

  output_stream_ << "Estado: " << description.GetState()
                 << "    Cadena pendiente: " << remaining_input
                 << "    Pila: " << description.GetStack()
                 << "    Transiciones aplicables: ";

  if (applicable_transitions.empty()) {
    output_stream_ << "ninguna";
  } else {
    bool is_first_transition = true;
    for (const Transition& transition : applicable_transitions) {
      if (!is_first_transition) output_stream_ << ", ";
      output_stream_ << GetTransitionNumber(transition);
      is_first_transition = false;
    }
  }
  output_stream_ << "\n";
}

void Tracer::ReportAppliedTransition(const Transition& transition) {
  if (!is_enabled_) return;
  output_stream_ << "  -> se aplica la transición "
                 << GetTransitionNumber(transition) << ": " << transition
                 << "\n";
}

void Tracer::ReportBacktracking() {
  if (!is_enabled_) return;
  output_stream_ << "  <- retroceso\n";
}

void Tracer::EndChain(bool accepted, unsigned long explored_descriptions) {
  if (!is_enabled_) return;
  output_stream_ << std::string(kSeparatorWidth, '-') << "\n";
  output_stream_ << " Cadena " << (accepted ? "ACEPTADA" : "RECHAZADA")
                 << " tras explorar " << explored_descriptions
                 << " descripciones instantáneas.\n";
  output_stream_ << std::string(kSeparatorWidth, '=') << "\n";
}