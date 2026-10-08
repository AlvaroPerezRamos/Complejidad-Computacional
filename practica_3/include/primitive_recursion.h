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
 * @file primitive_recursion.h
 * @brief Definición de la clase PrimitiveRecursion: la operación de recursión
 * primitiva. Dadas g : ℕⁿ → ℕ y h : ℕⁿ⁺² → ℕ se define f : ℕⁿ⁺¹ → ℕ mediante
 *
 *     f(x, 0)    = g(x)                      (ecuación límite)
 *     f(x, S(y)) = h(x, y, f(x, y))          (ecuación de recursión)
 *
 * Decisión sobre el "−1": evaluar f(x, y) leyendo la ecuación de recursión
 * hacia atrás exigiría descomponer y = S(y'), es decir, restar 1. Aquí se
 * evita por completo: se evalúa de abajo arriba,
 *
 *     f(x,0) = g(x);  f(x,1) = h(x,0,f(x,0));  ...;  f(x,y) = h(x,y−1,f(x,y−1))
 *
 * con un contador de nivel que solo SUBE (0, 1, 2, ...). Es la misma función
 * y exactamente las mismas llamadas, pero sin ninguna resta y sin recursión
 * del lenguaje, de modo que no hay riesgo de desbordar la pila aunque y sea
 * grande.
 *
 * Limitación conocida: no existe ninguna función de aridad 0 en este proyecto
 * (Z, S, Pᵢⁿ y las composiciones tienen aridad >= 1), así que g siempre tiene
 * al menos un parámetro. La recursión sin parámetros (g constante, como en
 * pred) no está soportada todavía.
 */

#ifndef PRIMITIVE_RECURSION_H_
#define PRIMITIVE_RECURSION_H_

#include "primitive_recursive_function.h"

/**
 * @brief f definida por recursión primitiva a partir de g (caso base) y h
 * (paso recursivo). El último argumento de f es la variable de recursión y.
 */
class PrimitiveRecursion : public PrimitiveRecursiveFunction {
 public:
  /**
   * @param base_case Función g, de aridad n.
   * @param recursive_step Función h, de aridad n + 2.
   * @throw InvalidFunctionDefinitionError Si alguna es nula o si la aridad de
   * h no es la de g más dos.
   */
  PrimitiveRecursion(FunctionPointer base_case, FunctionPointer recursive_step);

 protected:
  /**
   * @brief Evalúa f(x, y) de abajo arriba.
   *
   * Recuento de llamadas: la definición recursiva literal invoca f en los
   * niveles y, y−1, ..., 0 (y+1 invocaciones), g una vez y h y veces. Aquí la
   * invocación de f(x, y) la anota Evaluate(); las de los niveles 0..y−1 se
   * anotan explícitamente en el bucle, para que el total coincida con el de la
   * definición literal.
   */
  Natural Compute(const Arguments& arguments,
                  CallCounter& call_counter) const override;

 private:
  FunctionPointer base_case_;       // g
  FunctionPointer recursive_step_;  // h
};

#endif  // PRIMITIVE_RECURSION_H_
