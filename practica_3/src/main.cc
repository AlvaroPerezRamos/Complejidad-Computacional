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
 * @file main.cc
 * @brief Programa principal: CommandLineOptions -> FunctionLibrary::Power()
 * -> Evaluate. Muestra potencia(x, y) y el número de llamadas a funciones.
 *
 * Códigos de salida: 0 correcto; 1 uso incorrecto de la línea de comandos;
 * 2 desbordamiento (el resultado no cabe en 64 bits); 3 cualquier otro error.
 */

#include <iostream>
#include <string>

#include "../include/call_counter.h"
#include "../include/command_line_options.h"
#include "../include/errors.h"
#include "../include/function_library.h"

int main(int argc, char* argv[]) {
  try {
    const CommandLineOptions options = CommandLineOptions::Parse(argc, argv);

    if (options.IsHelpRequested()) {
      std::cout << CommandLineOptions::BuildHelpText(argc > 0 ? argv[0]
                                                              : "potencia");
      return 0;
    }

    const FunctionPointer power_function = FunctionLibrary::Power();
    CallCounter call_counter;
    const Natural result = power_function->Evaluate(
        {options.GetBase(), options.GetExponent()}, call_counter);

    std::cout << "potencia(" << options.GetBase() << ", "
              << options.GetExponent() << ") = " << result << "\n";
    std::cout << "Número de llamadas a funciones: " << call_counter.GetCalls()
              << "\n";

  } catch (const CommandLineError& error) {
    std::cerr << "Error: " << error.what() << "\n";
    return 1;
  } catch (const NaturalOverflowError& error) {
    std::cerr << "Error: " << error.what() << "\n";
    return 2;
  } catch (const Error& error) {
    std::cerr << "Error: " << error.what() << "\n";
    return 3;
  }

  return 0;
}