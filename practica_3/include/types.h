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
 * @brief Alias de tipos compartidos por todo el proyecto.
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <vector>

/**
 * Número natural (0, 1, 2, ...). Se usa un entero SIN signo de 64 bits: el
 * tipo no puede representar valores negativos, de modo que "resta" nunca
 * produce un resultado fuera de ℕ por construcción del tipo. El desbordamiento
 * por arriba se detecta en la función sucesor (ver SuccessorFunction).
 */
using Natural = unsigned long long;

/** Tupla de argumentos (x1, ..., xn) ∈ ℕⁿ con la que se evalúa una función. */
using Arguments = std::vector<Natural>;

#endif  // TYPES_H_
