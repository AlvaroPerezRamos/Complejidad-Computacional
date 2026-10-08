cmake_minimum_required(VERSION 3.10)

project(potencia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Sin tipo de compilación explícito se usa Release: la evaluación hace
# millones de llamadas y sin optimizar tarda varias veces más.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Fijado ANTES de add_executable(): CMake calcula el directorio de salida
# del target en el momento en que se declara, no al final del fichero.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# file(GLOB) se evalúa al CONFIGURAR: al añadir un .cc nuevo hay que volver a
# ejecutar "cmake -S . -B build" para que lo recoja.
file(GLOB SOURCES "src/*.cc")

add_executable(potencia ${SOURCES})
target_include_directories(potencia PRIVATE include)
