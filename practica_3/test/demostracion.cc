/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 3: Funciones primitivas recursivas de números naturales
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 13/10/2026
 * @file demostracion.cc
 * @brief Programa de demostración: llama a cada función con sus argumentos
 * escritos en la propia llamada y muestra el resultado y el número de llamadas
 * de esa evaluación. Sirve para ver de un vistazo cómo se usa la librería; el
 * programa de la práctica (potencia -x <n> -y <n>) es src/main.cc.
 */

#include <iostream>
#include <string>

#include "../include/call_counter.h"
#include "../include/errors.h"
#include "../include/function_library.h"
#include "../include/types.h"

namespace {

/** @brief Escribe los argumentos con el formato "(3, 4)". */
std::string BuildArgumentsText(const Arguments& arguments) {
  std::string arguments_text = "(";
  for (std::size_t position = 0; position < arguments.size(); ++position) {
    if (position > 0) {
      arguments_text += ", ";
    }
    arguments_text += std::to_string(arguments[position]);
  }
  return arguments_text + ")";
}

/**
 * @brief Evalúa una función en unos argumentos y muestra el resultado y las
 * llamadas que ha costado esa evaluación (el contador se reinicia antes).
 * @param function_name Nombre con el que se muestra la función.
 * @param function Función a evaluar.
 * @param arguments Argumentos de la llamada.
 */
void ShowCall(const std::string& function_name, const FunctionPointer& function,
              const Arguments& arguments) {
  CallCounter call_counter;
  const Natural result = function->Evaluate(arguments, call_counter);
  std::cout << function_name << BuildArgumentsText(arguments) << " = " << result
            << "   [llamadas: " << call_counter.GetCalls() << "]\n";
}

}  // namespace

int main() {
  const FunctionPointer zero = FunctionLibrary::Zero();
  const FunctionPointer successor = FunctionLibrary::Successor();
  const FunctionPointer second_of_three = FunctionLibrary::Projection(2, 3);
  const FunctionPointer one = FunctionLibrary::One();
  const FunctionPointer addition = FunctionLibrary::Addition();
  const FunctionPointer multiplication = FunctionLibrary::Multiplication();
  const FunctionPointer power = FunctionLibrary::Power();

  try {
    std::cout << "Funciones básicas\n";
    ShowCall("Z", zero, {7});
    ShowCall("S", successor, {4});
    ShowCall("P_2^3", second_of_three, {10, 20, 30});

    std::cout << "\nFunciones construidas\n";
    ShowCall("uno", one, {5});
    ShowCall("suma", addition, {3, 4});
    ShowCall("producto", multiplication, {2, 3});
    ShowCall("potencia", power, {2, 3});
    ShowCall("potencia", power, {5, 0});
  } catch (const Error& error) {
    std::cerr << "Error: " << error.what() << "\n";
    return 1;
  }
  return 0;
}