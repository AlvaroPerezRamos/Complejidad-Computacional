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
 * @file simulator.h
 * @brief Definición de Simulator: ejecuta la MT sobre una cadena. Sin
 * backtracking ni límite de pasos (determinista; el enunciado fija el
 * criterio de parada como "ausencia de transiciones", sin más).
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "chain.h"
#include "turing_machine.h"
#include "turing_run_result.h"

class Simulator {
 public:
  explicit Simulator(const TuringMachine& machine) : machine_(machine) {}

  /**
   * @brief Ejecuta la MT con chain en la cinta 1 (el resto en blanco) hasta
   * que no haya ninguna transición aplicable.
   */
  TuringRunResult Run(const Chain& chain) const;

 private:
  TuringMachine machine_;
};

#endif  // SIMULATOR_H_
