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
 * @file transition.h
 * @brief Definición de la clase Transition y de la clave TransitionKey.
 *
 * Transition no valida nada: no comprueba que sus estados pertenezcan a Q,
 * que sus símbolos pertenezcan a Σ/Γ, ni que la secuencia a apilar use
 * símbolos declarados en Γ. Es una quíntupla y nada más; esas
 * comprobaciones semánticas son responsabilidad de quien la construya a
 * partir del fichero de configuración (AutomatonParser/AutomatonValidator,
 * todavía sin implementar).
 *
 * Historial de versiones
 *   19/09/2026 - Creación del fichero e implementación completa: no ha
 *                habido una fase previa de solo esqueleto, a diferencia
 *                de Symbol/State/Alphabet/Chain, que partían de ficheros
 *                ya creados el 18/09/2026.
 *   23/09/2026 - Ampliación con operator<, necesario para Tracer.
 */

#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <iostream>
#include <string>
#include <tuple>

#include "state.h"
#include "symbol.h"

/**
 * @struct TransitionKey
 * @brief Identifica una entrada de la función de transición: el estado de
 * origen, el símbolo de entrada consultado y el símbolo de la cima de la
 * pila consultado. Puede haber más de una Transition asociada a la misma
 * clave (no determinismo): la clave agrupa, no distingue de forma única.
 */
struct TransitionKey {
  State origin_state;
  Symbol input_symbol;
  Symbol stack_symbol;

  /** @brief Operador menor que, necesario para usar TransitionKey como clave de
   * un std::map. */
  bool operator<(const TransitionKey& other) const {
    return std::tie(origin_state, input_symbol, stack_symbol) <
           std::tie(other.origin_state, other.input_symbol, other.stack_symbol);
  }
};

/**
 * @class Transition
 * @brief Representa una única transición: (origen, entrada, cima) ->
 * (destino, secuencia a apilar).
 */
class Transition {
 public:
  /**
   * @brief Construye una transición a partir de sus cinco componentes.
   * @param origin_state Estado de origen.
   * @param input_symbol Símbolo de entrada consultado (puede ser ε).
   * @param stack_symbol Símbolo de la cima de la pila consultado (nunca ε).
   * @param destination_state Estado de destino.
   * @param symbols_to_push Secuencia a apilar, ya normalizada: cadena
   * vacía si la transición no apila nada (en el fichero se escribe como
   * '.'), o los símbolos a apilar en orden, sin espacios.
   */
  Transition(const State& origin_state, const Symbol& input_symbol,
             const Symbol& stack_symbol, const State& destination_state,
             const std::string& symbols_to_push)
      : origin_state_(origin_state),
        input_symbol_(input_symbol),
        stack_symbol_(stack_symbol),
        destination_state_(destination_state),
        symbols_to_push_(symbols_to_push) {}

  /** @brief Destructor por defecto. */
  ~Transition() = default;

  /** @brief Devuelve el estado de origen. */
  const State& GetOriginState() const { return origin_state_; }

  /** @brief Devuelve el símbolo de entrada consultado. */
  const Symbol& GetInputSymbol() const { return input_symbol_; }

  /** @brief Devuelve el símbolo de la cima de la pila consultado. */
  const Symbol& GetStackSymbol() const { return stack_symbol_; }

  /** @brief Devuelve el estado de destino. */
  const State& GetDestinationState() const { return destination_state_; }

  /** @brief Devuelve la secuencia a apilar (vacía si la transición no apila
   * nada). */
  const std::string& GetSymbolsToPush() const { return symbols_to_push_; }

  /** @brief Devuelve la clave bajo la que se indexa esta transición en
   * TransitionFunction. */
  TransitionKey GetKey() const {
    return TransitionKey{origin_state_, input_symbol_, stack_symbol_};
  }

  /** @brief Indica si el símbolo de entrada consultado es ε. */
  bool IsEpsilonTransition() const { return input_symbol_.IsEpsilon(); }

  /** @brief Representación legible de la transición, p. ej. "δ(p, a, S) ∋ (p,
   * AS)". */
  std::string ToString() const;

  /** @brief Operador de igualdad: misma quíntupla exacta. */
  bool operator==(const Transition& other) const {
    return origin_state_ == other.origin_state_ &&
           input_symbol_ == other.input_symbol_ &&
           stack_symbol_ == other.stack_symbol_ &&
           destination_state_ == other.destination_state_ &&
           symbols_to_push_ == other.symbols_to_push_;
  }

  /** @brief Operador de desigualdad. */
  bool operator!=(const Transition& other) const { return !(*this == other); }

  /**
   * @brief Orden total arbitrario entre transiciones (por sus cinco
   * campos). No tiene ningún significado en la teoría de autómatas:
   * existe solo para poder usar Transition como clave de un std::map
   * (Tracer la necesita para asociar cada transición con su número).
   */
  bool operator<(const Transition& other) const {
    return std::tie(origin_state_, input_symbol_, stack_symbol_,
                    destination_state_, symbols_to_push_) <
           std::tie(other.origin_state_, other.input_symbol_,
                    other.stack_symbol_, other.destination_state_,
                    other.symbols_to_push_);
  }

  /** @brief Sobrecarga del operador de salida (usa ToString()). */
  friend std::ostream& operator<<(std::ostream& output_stream,
                                  const Transition& transition);

 private:
  State origin_state_;
  Symbol input_symbol_;
  Symbol stack_symbol_;
  State destination_state_;
  std::string symbols_to_push_;
};

#endif  // TRANSITION_H_