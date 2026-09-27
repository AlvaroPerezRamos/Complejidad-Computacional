/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 18/09/2026
 * @file state.h
 * @brief Definición de la clase State.
 *
 * La pertenencia a q0/F se consulta en PushdownAutomaton, no aquí.
*/

#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <string>

/**
 * @class State
 * @brief Representa un estado de un autómata, identificado únicamente por
 * su nombre.
 */
class State {
 public:
  /** @brief Constructor por defecto, necesario para usar State como valor por
   * defecto en contenedores. */
  State() = default;

  /**
   * @brief Construye un estado a partir de su nombre.
   * @param name Nombre del estado, tal y como aparece en el fichero de
   * configuración.
   */
  explicit State(const std::string& name) : name_(name) {}

  /** @brief Destructor por defecto. */
  ~State() = default;

  /** @brief Devuelve el nombre del estado. */
  const std::string& GetName() const { return name_; }

  /** @brief Operador menor que, necesario para almacenar estados en
   * std::set/std::map. */
  bool operator<(const State& other) const { return name_ < other.name_; }

  /** @brief Operador de igualdad entre estados. */
  bool operator==(const State& other) const { return name_ == other.name_; }

  /** @brief Operador de desigualdad entre estados. */
  bool operator!=(const State& other) const { return !(*this == other); }

  /** @brief Sobrecarga del operador de salida. */
  friend std::ostream& operator<<(std::ostream& output_stream,
                                  const State& state) {
    output_stream << state.name_;
    return output_stream;
  }

 private:
  std::string name_; /**< Nombre del estado. */
};

#endif  // STATE_H_