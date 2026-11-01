
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
├── scripts/
│   └── graficar.cpp
│
├── resultados/          (generado por el benchmark)
│   ├── tiempos.csv
│   └── grafica_tiempos.svg
│
├── CMakeLists.txt
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
| `graficar.cpp` | Genera la gráfica (SVG) a partir del CSV del benchmark. |
| `CMakeLists.txt` | Configuración de compilación con CMake (tres ejecutables: `pruebas`, `benchmark` y `graficar`). |
| `decisiones_implementacion.md` | Documenta las decisiones técnicas tomadas durante el desarrollo. |

## 5. Compilación y ejecución

Todos los comandos se ejecutan desde la raíz del repositorio. Se requiere un compilador con soporte de C++17 (GCC, Clang o MinGW en Windows). Se generan tres ejecutables: las **pruebas de correctitud**, el **experimento de tiempos** y el **generador de la gráfica**.

### Opción A: g++ directamente

```bash
mkdir -p build      # en PowerShell: mkdir build -Force

# Pruebas de correctitud (Karatsuba vs. clásica y casos de borde)
g++ -std=c++17 -O2 -Wall -Isrc tests/pruebas.cpp src/operaciones.cpp src/multiplicacion_clasica.cpp src/karatsuba.cpp -o build/pruebas
./build/pruebas

# Experimento de tiempos (2 a 4096 dígitos)
g++ -std=c++17 -O2 -Wall -Isrc src/main.cpp src/operaciones.cpp src/multiplicacion_clasica.cpp src/karatsuba.cpp -o build/benchmark
./build/benchmark

# Grafica a partir de resultados/tiempos.csv
g++ -std=c++17 -O2 -Wall scripts/graficar.cpp -o build/graficar
./build/graficar
```

En Windows los ejecutables quedan como `build\pruebas.exe`, `build\benchmark.exe` y `build\graficar.exe`.

### Opción B: CMake (CLion, Visual Studio Code, terminal)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/pruebas
./build/benchmark
./build/graficar
```

En CLion basta con abrir la carpeta del repositorio. Para medir tiempos se debe usar el perfil **Release**, porque el perfil Debug compila sin optimización y los tiempos no son representativos.

### Salidas

- `build/pruebas` imprime los casos correctos de cada categoría y el total. Termina con código 0 si todas las pruebas pasan.
- `build/benchmark` imprime la tabla de tiempos y el punto de cruce, y guarda los datos en `resultados/tiempos.csv`. Tarda alrededor de un minuto.
- `build/graficar` lee `resultados/tiempos.csv` y genera `resultados/grafica_tiempos.svg` (se abre en cualquier navegador y se puede insertar en Word). Además imprime el punto de cruce y el exponente empírico de cada algoritmo (pendiente en escala log-log).

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
