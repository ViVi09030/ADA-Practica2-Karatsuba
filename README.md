
# Práctica 2 — Multiplicación rápida de enteros: Algoritmo de Karatsuba

## 1. Descripción

Proyecto desarrollado para la asignatura **Análisis y Diseño de Algoritmos**. Se implementan y comparan dos algoritmos de multiplicación de enteros de longitud arbitraria: el método clásico y el algoritmo de Karatsuba, utilizando la estrategia de **Divide y Vencerás**.

Los números se representan mediante cadenas de caracteres (`std::string`) en base decimal, evitando las limitaciones de los tipos numéricos nativos para representar operandos completos.

## 2. Objetivos

- Implementar la multiplicación clásica con complejidad temporal Θ(n²).
- Implementar el algoritmo recursivo de Karatsuba.
- Desarrollar operaciones auxiliares sobre cadenas numéricas.
- Analizar la recurrencia de Karatsuba mediante el Teorema Maestro y el árbol de recurrencia.
- Comparar experimentalmente el rendimiento de ambos algoritmos.

## 3. Tecnologías utilizadas

- **Lenguaje:** C++
- **Estándar:** C++17
- **Compilador:** Apple Clang / GCC
- **Control de versiones:** Git y GitHub
- **Medición de tiempos:** `std::chrono::steady_clock`

## 4. Estructura del proyecto

```text
ADA-Practica2-Karatsuba/
│
├── src/
│   ├── operaciones.h
│   ├── operaciones.cpp
│   ├── multiplicacion_clasica.h
│   ├── multiplicacion_clasica.cpp
│   ├── karatsuba.h
│   ├── karatsuba.cpp
│   └── main.cpp
│
├── tests/
│   └── pruebas.cpp
│
├── docs/
│   └── decisiones_implementacion.md
│
├── .gitignore
└── README.md
```

### Descripción de los archivos

| Archivo | Descripción |
|---|---|
| `operaciones.h` | Contiene las declaraciones de las operaciones auxiliares. |
| `operaciones.cpp` | Implementa la suma, resta, desplazamiento, partición y normalización de números. |
| `multiplicacion_clasica.h` | Contiene la declaración de la multiplicación clásica. |
| `multiplicacion_clasica.cpp` | Implementa el algoritmo de multiplicación clásica. |
| `karatsuba.h` | Contiene la declaración de la función de Karatsuba. |
| `karatsuba.cpp` | Implementa el algoritmo recursivo de Karatsuba. |
| `main.cpp` | Contiene el punto de entrada e integración del programa. |
| `pruebas.cpp` | Contiene las pruebas de correctitud y los casos límite. |
| `decisiones_implementacion.md` | Documenta las decisiones técnicas tomadas durante el desarrollo. |

## 5. Compilación y ejecución

La compilación se realizará desde la raíz del repositorio utilizando el estándar C++17.

Una vez integrados los componentes del proyecto, se incluirán los comandos definitivos de compilación y ejecución.

## 6. Organización del equipo

El desarrollo se distribuye entre tres integrantes:

- **Integrante 1:** representación de datos, operaciones auxiliares y multiplicación clásica.
- **Integrante 2:** implementación de Karatsuba y análisis matemático.
- **Integrante 3:** validación de correctitud, experimentación, benchmarks y edición final del informe.

## 7. Control de versiones

El proyecto utiliza Git y GitHub para gestionar el desarrollo colaborativo.

Se emplean ramas independientes para organizar el trabajo de cada integrante. Los cambios se integran en la rama `main` mediante Pull Requests y commits descriptivos.

## 8. Uso ético de inteligencia artificial

El equipo documentará las herramientas de inteligencia artificial utilizadas como apoyo para comprender conceptos, resolver dudas técnicas, depurar errores y mejorar la documentación.

Todos los integrantes deben comprender y ser capaces de explicar el código y las decisiones de implementación realizadas durante el desarrollo de la práctica.