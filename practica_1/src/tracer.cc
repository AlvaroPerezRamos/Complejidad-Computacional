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
 *   23/09/2026 - Ampliación, el mismo día: agrupación de retrocesos
 *                consecutivos en una sola línea.
 *   23/09/2026 - Rediseño, el mismo día: IDs de descripción en vez de un
 *                simple contador de retrocesos.
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

void Tracer::FlushPendingBacktracks() {
  if (!has_pending_backtrack_) {
    return;
  }
  const unsigned long current_id =
      description_id_stack_.empty() ? 0 : description_id_stack_.back();
  output_stream_ << "  <- retroceso: de la descripción "
                 << pending_backtrack_from_id_ << " a la " << current_id
                 << "\n";
  has_pending_backtrack_ = false;
}

void Tracer::BeginChain(const Chain& chain) {
  if (!is_enabled_) return;
  description_id_stack_.clear();
  next_description_id_ = 1;
  has_pending_backtrack_ =
      false;  // Descarta lo pendiente de una cadena anterior abortada.

  output_stream_ << std::string(kSeparatorWidth, '=') << "\n";
  output_stream_ << " Traza del reconocimiento de la cadena: " << chain << "\n";
  output_stream_ << std::string(kSeparatorWidth, '=') << "\n";
}

void Tracer::ReportDescription(
    const InstantaneousDescription& description,
    const std::vector<Transition>& applicable_transitions) {
  if (!is_enabled_) return;
  FlushPendingBacktracks();

  const unsigned long description_id = next_description_id_++;
  description_id_stack_.push_back(description_id);
  last_applicable_transitions_count_ = applicable_transitions.size();

  const std::string remaining_input = description.GetRemainingInput().empty()
                                          ? "ε"
                                          : description.GetRemainingInput();

  output_stream_ << "ID: " << description_id
                 << "    Estado: " << description.GetState()
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
  if (last_applicable_transitions_count_ <= 1) {
    return;  // Única transición aplicable: no era una elección real, no hace
             // falta anunciarla.
  }
  FlushPendingBacktracks();
  output_stream_ << "  -> se aplica la transición "
                 << GetTransitionNumber(transition) << ": " << transition
                 << "\n";
}

void Tracer::ReportBacktracking() {
  if (!is_enabled_) return;
  if (description_id_stack_.empty()) {
    return;  // Salvaguarda: no debería llamarse sin ninguna descripción
             // abierta.
  }

  const unsigned long abandoned_id = description_id_stack_.back();
  description_id_stack_.pop_back();

  if (!has_pending_backtrack_) {
    pending_backtrack_from_id_ = abandoned_id;
    has_pending_backtrack_ = true;
  }
  // Si ya había un retroceso pendiente, pending_backtrack_from_id_ se queda
  // como estaba: es el primero (más profundo) de la racha actual, que es
  // el dato que hace falta para el mensaje final ("de X a Y").
}

void Tracer::EndChain(bool accepted, unsigned long explored_descriptions) {
  if (!is_enabled_) return;
  FlushPendingBacktracks();

  const bool is_singular = (explored_descriptions == 1);
  output_stream_ << std::string(kSeparatorWidth, '-') << "\n";
  output_stream_ << " Cadena " << (accepted ? "ACEPTADA" : "RECHAZADA")
                 << " tras explorar " << explored_descriptions << " "
                 << (is_singular ? "descripción instantánea"
                                 : "descripciones instantáneas")
                 << ".\n";
  output_stream_ << std::string(kSeparatorWidth, '=') << "\n";
}