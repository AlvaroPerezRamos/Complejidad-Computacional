cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(automata_pila)

# Configuración del compilador
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Directorios de los archivos fuente y encabezados
set(SRC_DIR src)

# Carpeta de salida para el binario: debe fijarse ANTES de
# add_executable(), CMake calcula el directorio de salida del target en
# el momento en que se declara, no al final del fichero.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Buscar todos los archivos .cc en el directorio src
file(GLOB SOURCES "${SRC_DIR}/*.cc")

# Definir el ejecutable
add_executable(automata_pila ${SOURCES})