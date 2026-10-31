cmake_minimum_required(VERSION 3.16)
project(ADA_Practica2_Karatsuba CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(FUENTES_COMUNES
    src/operaciones.cpp
    src/multiplicacion_clasica.cpp
    src/karatsuba.cpp
)

add_executable(pruebas tests/pruebas.cpp ${FUENTES_COMUNES})
target_include_directories(pruebas PRIVATE src)

add_executable(benchmark src/main.cpp ${FUENTES_COMUNES})
target_include_directories(benchmark PRIVATE src)

if(NOT MSVC)
    target_compile_options(benchmark PRIVATE -O2)
endif()

target_compile_definitions(benchmark PRIVATE RAIZ_PROYECTO="${CMAKE_SOURCE_DIR}")

add_executable(graficar scripts/graficar.cpp)
target_compile_definitions(graficar PRIVATE RAIZ_PROYECTO="${CMAKE_SOURCE_DIR}")
