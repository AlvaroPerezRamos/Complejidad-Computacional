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
 * @file state.h
 * @brief Definición de la clase State. La pertenencia a q0/F se consulta
 * en TuringMachine, no aquí.
 */

#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <string>

/** @brief Estado de la máquina, identificado por su nombre. */
class State {
 public:
  /** @brief Constructor por defecto (para usar State como valor en contenedores). */
  State() = default;

  explicit State(const std::string& name) : name_(name) {}
  ~State() = default;

  const std::string& GetName() const { return name_; }

  bool operator<(const State& other) const { return name_ < other.name_; }
  bool operator==(const State& other) const { return name_ == other.name_; }
  bool operator!=(const State& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& output_stream, const State& state) {
    output_stream << state.name_;
    return output_stream;
  }

 private:
  std::string name_;
};

#endif  // STATE_H_
