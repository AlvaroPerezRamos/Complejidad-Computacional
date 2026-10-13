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
 * Se evalúa de abajo arriba,
 *
 *     f(x,0) = g(x);  f(x,1) = h(x,0,f(x,0));  ...;  f(x,y) = h(x,y−1,f(x,y−1))
 *
 * con un contador de nivel que sube (0, 1, 2, ...). Es la misma función y las
 * mismas llamadas que la definición recursiva literal, pero sin recursión del
 * lenguaje, de modo que no hay riesgo de desbordar la pila aunque y sea grande.
 *
 * Recuento de llamadas: la recursión NO anota llamada propia al entrar; solo
 * anota sus llamadas recursivas (una por nivel, y en total). La entrada en
 * la función (suma, producto...) la anota NamedFunction.
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
   * niveles y, y−1, ..., 0 (y+1 invocaciones), g una vez y h y veces. La
   * invocación de nivel y es la entrada en la función y la anota NamedFunction;
   * las de los niveles y−1, ..., 0 son las y llamadas recursivas, que se
   * anotan explícitamente en el bucle.
   */
  Natural Compute(const Arguments& arguments,
                  CallCounter& call_counter) const override;

 private:
  FunctionPointer base_case_;       // g
  FunctionPointer recursive_step_;  // h
};

#endif  // PRIMITIVE_RECURSION_H_