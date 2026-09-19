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
 * @file config_reader.h
 * @brief Funciones para leer y trocear el fichero de configuración.
 *
 * Módulo provisional: son exactamente las funciones libres que antes
 * vivían dentro de main.cc, movidas aquí para que main.cc se quede solo
 * con el punto de entrada. Cuando se implemente AutomatonParser, esta
 * lógica se convertirá en un conjunto de métodos privados de esa clase
 * (tal y como se acordó en el diseño); hasta entonces se mantiene como
 * funciones libres, sin agruparlas todavía en una clase, para no
 * adelantar ese diseño antes de tiempo.
 *
 * Historial de versiones
 *   19/09/2026 - Creación del fichero y traslado de las funciones libres de main.cc a este módulo.
 */

#ifndef CONFIG_READER_H_
#define CONFIG_READER_H_

#include <fstream>
#include <string>
#include <vector>

#include "state.h"
#include "symbol.h"
#include "transition.h"

/**
 * @brief Elimina el comentario de una línea del fichero de configuración:
 * todo lo que sigue al primer '#', según el convenio de la sección 3 del
 * enunciado.
 * @param line Línea tal y como se ha leído del fichero.
 * @return La línea sin su comentario.
 */
std::string StripComment(const std::string& line);

/**
 * @brief Indica si una línea, una vez eliminado su comentario, no
 * contiene ningún carácter que no sea espacio en blanco.
 * @param line Línea a comprobar.
 * @return true si la línea está en blanco, false en caso contrario.
 */
bool IsBlankLine(const std::string& line);

/**
 * @brief Lee la siguiente línea significativa del fichero, saltando
 * comentarios y líneas en blanco.
 * @param config_file Fichero de configuración abierto.
 * @param significant_line Donde se deja la línea leída (sin comentario).
 * @return true si se ha encontrado una línea significativa, false si se
 * ha llegado al final del fichero.
 */
bool ReadNextSignificantLine(std::ifstream& config_file,
                             std::string& significant_line);

/**
 * @brief Divide una línea en los tokens separados por espacios en blanco
 * que contiene.
 * @param line Línea a dividir.
 * @return Los tokens, en el orden en que aparecen en la línea.
 */
std::vector<std::string> SplitIntoTokens(const std::string& line);

/**
 * @brief Construye el conjunto de estados (Q, o también F) a partir de su
 * línea del fichero de configuración.
 * @param states_line Línea con los nombres de los estados, separados por
 * espacios.
 * @return Los estados en el orden en que aparecen en la línea.
 */
std::vector<State> ParseStates(const std::string& states_line);

/**
 * @brief Construye un único estado a partir de su línea (usado para q0,
 * que debe ser exactamente un token).
 * @param line Línea con el nombre del estado inicial.
 * @throw MissingSectionError Si la línea no tiene exactamente un token
 * (el enunciado exige detectar "más de un estado inicial" como error).
 */
State ParseSingleState(const std::string& line);

/**
 * @brief Construye un único símbolo a partir de su línea (usado para Z0,
 * que debe ser exactamente un token de un carácter).
 * @param line Línea con el símbolo inicial de la pila.
 * @throw MissingSectionError Si la línea no tiene exactamente un token.
 * @throw InvalidSymbolError Si el token no tiene un único carácter.
 */
Symbol ParseSingleSymbol(const std::string& line);

/**
 * @brief Construye una Transition a partir de su línea del fichero de
 * configuración: origen, símbolo de entrada, símbolo de la cima, destino
 * y secuencia a apilar.
 * @param transition_line Línea con los cinco campos de la transición.
 * @throw InvalidTransitionError Si la línea no tiene exactamente cinco
 * campos, si el símbolo de entrada o la cima consultada no son un único
 * carácter, o si la cima consultada es ε (nunca puede serlo).
 */
Transition ParseTransitionLine(const std::string& transition_line);

#endif  // CONFIG_READER_H_