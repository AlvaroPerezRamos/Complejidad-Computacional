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
 * @file call_counter.h
 * @brief Definición de la clase CallCounter.
 */

#ifndef CALL_COUNTER_H_
#define CALL_COUNTER_H_

/**
 * @class CallCounter
 * @brief Cuenta cuántas llamadas a funciones se realizan durante una
 * evaluación.
 *
 * No es un contador global (estático): se pasa explícitamente a cada
 * evaluación, de modo que dos evaluaciones independientes no se contaminan y
 * el comportamiento es fácil de probar.
 */
class CallCounter {
 public:
  /** @brief Anota una llamada más. */
  void RegisterCall() { ++calls_; }

  /** @brief Devuelve el total de llamadas anotadas. */
  unsigned long long GetCalls() const { return calls_; }

  /** @brief Pone el contador a cero. */
  void Reset() { calls_ = 0; }

 private:
  unsigned long long calls_ = 0; /**< Llamadas anotadas hasta ahora. */
};

#endif  // CALL_COUNTER_H_
