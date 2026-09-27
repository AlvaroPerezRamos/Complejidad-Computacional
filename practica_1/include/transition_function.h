/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Asignatura: Complejidad Computacional
 * Curso: 4º
 * Práctica 1: Simulador de un autómata con pila
 *
 * @author Álvaro Pérez Ramos - alu0101574042@ull.edu.es
 * @date 19/09/2026
 * @file transition_function.h
 * @brief Definición de la clase TransitionFunction (la función δ completa).
 */

#ifndef TRANSITION_FUNCTION_H_
#define TRANSITION_FUNCTION_H_

#include <cstddef>
#include <map>
#include <vector>

#include "state.h"
#include "symbol.h"
#include "transition.h"

/**
 * @class TransitionFunction
 * @brief Representa la función de transición (δ) completa de un autómata
 * con pila.
 */
class TransitionFunction {
 public:
  /** @brief Construye una función de transición sin ninguna transición. */
  TransitionFunction() = default;

  /** @brief Destructor por defecto. */
  ~TransitionFunction() = default;

  /**
   * @brief Inserta una transición.
   * @param transition Transición a insertar.
   * @return true si es una transición nueva; false si ya existía
   * exactamente la misma quíntupla (mismo origen, entrada, cima, destino y
   * secuencia a apilar), en cuyo caso no se modifica nada. El enunciado
   * trata una transición repetida como un aviso, no como un error: es
   * responsabilidad de quien llama a Insert() decidir si informa de ello.
   */
  bool Insert(const Transition& transition);

  /**
   * @brief Devuelve las transiciones aplicables desde una descripción
   * instantánea concreta: mismo estado de origen, mismo símbolo de
   * entrada consultado y misma cima de pila consultada.
   *
   * Para obtener también las ε-transiciones aplicables (que se consultan
   * con independencia del símbolo de entrada restante) hay que llamar a
   * este método una segunda vez con el símbolo ε como símbolo de entrada;
   * unir ambos resultados es responsabilidad de Simulator, no de esta
   * clase.
   * @param origin_state Estado de origen a consultar.
   * @param input_symbol Símbolo de entrada consultado.
   * @param stack_symbol Símbolo de la cima de la pila consultado.
   * @return Las transiciones aplicables, o un vector vacío si no hay
   * ninguna.
   */
  std::vector<Transition> GetApplicableTransitions(
      const State& origin_state, const Symbol& input_symbol,
      const Symbol& stack_symbol) const;

  /**
   * @brief Devuelve todas las transiciones cuyo estado de origen es
   * origin_state, en el orden en que se insertaron.
   * @param origin_state Estado de origen a consultar.
   */
  std::vector<Transition> GetTransitionsFrom(const State& origin_state) const;

  /**
   * @brief Devuelve todas las transiciones ordenadas para numerarlas en la
   * traza: agrupadas por estado de origen según state_declaration_order
   * (empezando por q0); dentro de cada estado, primero sus autobucles y
   * después el resto, preservando en ambos grupos el orden en que
   * aparecieron en el fichero de configuración.
   * @param state_declaration_order Estados en el orden en que se
   * declararon en la línea Q del fichero de configuración.
   */
  std::vector<Transition> GetOrderedTransitions(
      const std::vector<State>& state_declaration_order) const;

  /** @brief Devuelve el número total de transiciones distintas insertadas. */
  std::size_t Size() const { return insertion_order_.size(); }

 private:
  std::map<TransitionKey, std::vector<Transition>> transitions_by_key_;
  std::vector<Transition> insertion_order_; /**< Transiciones en el orden en que
                                               se leyeron del fichero. */
};

#endif  // TRANSITION_FUNCTION_H_