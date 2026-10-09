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
 * @file function_library.h
 * @brief Definición de la clase FunctionLibrary: catálogo de las funciones
 * concretas de la práctica, construidas SOLO con las funciones básicas
 * (Z, S, Pᵢⁿ) y las operaciones de combinación, composición y recursión.
 * Aquí no hay ni una operación aritmética del lenguaje.
 *
 * Solo se construyen las funciones que potencia necesita (el enunciado pide
 * "todas aquellas que sean necesarias"): uno, suma, producto y potencia.
 *
 * suma y producto recurren sobre su segundo argumento, así que se pasa en esa
 * posición el operando pequeño: producto calcula suma(r, x) y potencia calcula
 * producto(r, x), con r el valor acumulado.
 *
 * Notación de los comentarios: f ∘ (g1, ..., gm) es la composición de f con
 * la combinación (g1, ..., gm), es decir, f aplicada a la tupla (g1(x), ...,
 * gm(x)).
 */

#ifndef FUNCTION_LIBRARY_H_
#define FUNCTION_LIBRARY_H_

#include <cstddef>

#include "primitive_recursive_function.h"

/** @brief Fábrica estática de las funciones primitivas recursivas de la
 * práctica. */
class FunctionLibrary {
 public:
  /** @brief Z : ℕ → ℕ, Z(x) = 0. */
  static FunctionPointer Zero();

  /** @brief S : ℕ → ℕ, S(x) = x + 1. */
  static FunctionPointer Successor();

  /** @brief Pᵢⁿ : ℕⁿ → ℕ, Pᵢⁿ(x1..xn) = xi (i desde 1). */
  static FunctionPointer Projection(std::size_t position, std::size_t arity);

  /**
   * @brief uno : ℕ → ℕ, uno(x) = 1.
   *
   *     uno = S ∘ (Z)
   */
  static FunctionPointer One();

  /**
   * @brief suma : ℕ² → ℕ, suma(x, y) = x + y.
   *
   *     suma(x, 0)    = P₁¹(x)
   *     suma(x, S(y)) = S ∘ (P₃³) (x, y, suma(x, y))
   */
  static FunctionPointer Addition();

  /**
   * @brief producto : ℕ² → ℕ, producto(x, y) = x · y.
   *
   *     producto(x, 0)    = Z(x)
   *     producto(x, S(y)) = suma ∘ (P₃³, P₁³) (x, y, producto(x, y))
   */
  static FunctionPointer Multiplication();

  /**
   * @brief potencia : ℕ² → ℕ, potencia(x, y) = xʸ  (con 0⁰ = 1).
   *
   *     potencia(x, 0)    = uno(x)
   *     potencia(x, S(y)) = producto ∘ (P₃³, P₁³) (x, y, potencia(x, y))
   */
  static FunctionPointer Power();
};

#endif  // FUNCTION_LIBRARY_H_