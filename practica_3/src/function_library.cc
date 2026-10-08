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
 * @file function_library.cc
 * @brief Implementación de la clase FunctionLibrary.
 */

#include "../include/function_library.h"

#include <memory>
#include <vector>

#include "../include/basic_functions.h"
#include "../include/combination.h"
#include "../include/composition.h"
#include "../include/primitive_recursion.h"

FunctionPointer FunctionLibrary::Zero() {
  return std::make_shared<const ZeroFunction>();
}

FunctionPointer FunctionLibrary::Successor() {
  return std::make_shared<const SuccessorFunction>();
}

FunctionPointer FunctionLibrary::Projection(std::size_t position,
                                            std::size_t arity) {
  return std::make_shared<const ProjectionFunction>(position, arity);
}

FunctionPointer FunctionLibrary::One() {
  // uno = S ∘ (Z)
  const Combination zero_combination(std::vector<FunctionPointer>{Zero()});
  return std::make_shared<const Composition>(Successor(), zero_combination);
}

FunctionPointer FunctionLibrary::Addition() {
  // g(x) = P₁¹(x)
  const FunctionPointer base_case = Projection(1, 1);
  // h(x, y, r) = S(P₃³(x, y, r)) = S(r)
  const Combination accumulated_combination(
      std::vector<FunctionPointer>{Projection(3, 3)});
  const FunctionPointer recursive_step =
      std::make_shared<const Composition>(Successor(), accumulated_combination);
  return std::make_shared<const PrimitiveRecursion>(base_case, recursive_step);
}

FunctionPointer FunctionLibrary::Multiplication() {
  // g(x) = Z(x)
  const FunctionPointer base_case = Zero();
  // h(x, y, r) = suma(P₁³(x, y, r), P₃³(x, y, r)) = x + r
  const Combination parameter_and_accumulated_combination(
      std::vector<FunctionPointer>{Projection(1, 3), Projection(3, 3)});
  const FunctionPointer recursive_step = std::make_shared<const Composition>(
      Addition(), parameter_and_accumulated_combination);
  return std::make_shared<const PrimitiveRecursion>(base_case, recursive_step);
}

FunctionPointer FunctionLibrary::Power() {
  // g(x) = uno(x)
  const FunctionPointer base_case = One();
  // h(x, y, r) = producto(P₁³(x, y, r), P₃³(x, y, r)) = x · r
  const Combination parameter_and_accumulated_combination(
      std::vector<FunctionPointer>{Projection(1, 3), Projection(3, 3)});
  const FunctionPointer recursive_step = std::make_shared<const Composition>(
      Multiplication(), parameter_and_accumulated_combination);
  return std::make_shared<const PrimitiveRecursion>(base_case, recursive_step);
}