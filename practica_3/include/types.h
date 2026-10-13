/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 3: Funciones primitivas recursivas de números naturales
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 08/10/2026
 * @file types.h
 * @brief Alias de tipos compartidos por todo el proyecto: Natural y Arguments.
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <vector>

/**
 * @brief Número natural (0, 1, 2, ...). Entero SIN signo de 64 bits.
 *
 * Con un tipo sin signo, 0 - 1 no da -1 sino 18446744073709551615 (da la
 * vuelta): una resta descuidada no falla, devuelve un valor enorme sin avisar.
 * Por eso en todo el proyecto no hay restas sobre valores de ℕ. El
 * desbordamiento por arriba se detecta en SuccessorFunction (paso 2).
 */
using Natural = unsigned long long;

/** @brief Tupla de argumentos (x1, ..., xn) ∈ ℕⁿ con la que se evalúa una
 * función. */
using Arguments = std::vector<Natural>;

#endif  // TYPES_H_