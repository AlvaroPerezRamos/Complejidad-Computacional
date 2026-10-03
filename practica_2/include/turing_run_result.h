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
 * @file turing_run_result.h
 * @brief Resultado de ejecutar la MT sobre una cadena: lo que el
 * enunciado exige mostrar siempre, se acepte o no.
 */

#ifndef TURING_RUN_RESULT_H_
#define TURING_RUN_RESULT_H_

#include <string>

/** @brief Veredicto y contenido final de la cinta 1 (con el cabezal entre corchetes). */
struct TuringRunResult {
  bool is_accepted;
  std::string tape_contents;
};

#endif  // TURING_RUN_RESULT_H_
