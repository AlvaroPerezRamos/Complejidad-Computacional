cmake_minimum_required(VERSION 3.10)

project(maquina_turing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Fijado ANTES de add_executable(): CMake calcula el directorio de salida
# del target en el momento en que se declara, no al final del fichero.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SOURCES "src/*.cc")

add_executable(maquina_turing ${SOURCES})
target_include_directories(maquina_turing PRIVATE include)