/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 3: Funciones primitivas recursivas de números naturales
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 09/10/2026
 * @file named_function.h
 * @brief Definición de la clase NamedFunction: una función primitiva
 * recursiva definida (uno, suma, producto, potencia) a la que se da nombre.
 *
 * La composición, la combinación y la recursión primitiva son OPERACIONES y
 * no anotan llamada por sí mismas. Una función definida con ellas (p. ej.
 * suma = Rec[P₁¹, S ∘ (P₃³)]) sí cuenta una llamada cada vez que se entra en
 * ella: eso es lo que añade NamedFunction. Su valor y su aridad son los de la
 * definición que envuelve.
 */

#ifndef NAMED_FUNCTION_H_
#define NAMED_FUNCTION_H_

#include <string>

#include "primitive_recursive_function.h"

/** @brief Función con nombre: anota una llamada y delega en su definición. */
class NamedFunction : public PrimitiveRecursiveFunction {
 public:
  /**
   * @param name Nombre de la función (p. ej. "suma").
   * @param definition Función (composición o recursión) que la define.
   * @throw InvalidFunctionDefinitionError Si la definición es nula.
   */
  NamedFunction(std::string name, FunctionPointer definition);

 protected:
  /** @brief Evalúa la definición con los mismos argumentos. */
  Natural Compute(const Arguments& arguments,
                  CallCounter& call_counter) const override;

 private:
  FunctionPointer definition_;
};

#endif  // NAMED_FUNCTION_H_