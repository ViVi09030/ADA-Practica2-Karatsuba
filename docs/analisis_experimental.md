# Análisis experimental (borrador)

## 1. Montaje experimental

**Equipo de medición.** [Procesador, memoria RAM, sistema operativo, compilador y versión, p. ej. "Intel Core i5-1135G7, 16 GB RAM, Windows 11, g++13.2 (MinGW)"]. Ambos programas se compilaron con `-std=c++17 -O2`.

**Entradas.** Para cada tamaño n = 2, 4, 8, …, 4096 dígitos se generaron 5 pares de números aleatorios de exactamente n dígitos (el primer dígito distinto de cero), con un generador `mt19937` de semilla fija, de modo que el experimento es reproducible. Los dos algoritmos se midieron sobre los mismos pares.

**Verificación previa.** Antes de medir cada tamaño se comprobó que `karatsuba` y `multiplicacionClasica` producen el mismo resultado para los 5 pares. La correctitud general se validó por separado con la batería de `tests/pruebas.cpp` ([N] casos, todos correctos), que cubre números de 1 dígito, ceros a la izquierda, ceros intercalados, longitudes impares antes del relleno, sumas x₁ + x₀ con dígito extra y productos de hasta 4096 dígitos.

**Medición.** Se usó `std::chrono::steady_clock`, un reloj monótono que no se ve afectado por cambios en la hora del sistema. Como una multiplicación de pocos dígitos tarda fracciones de microsegundo, muy por debajo de la resolución útil del reloj, no se mide una sola ejecución. Primero se calibra: el lote de 5 multiplicaciones se repite, duplicando el número de repeticiones hasta que el lote dure al menos 0,1 s. Luego se toman 5 mediciones independientes con ese número de repeticiones y se reporta el tiempo promedio por multiplicación y su desviación estándar. Antes de calibrar se ejecuta un lote de calentamiento.

**Evitar que el compilador elimine el cálculo.** Con `-O2` el compilador puede descartar un cálculo cuyo resultado no se usa. Por eso cada producto se "consume": su longitud y su último dígito se acumulan en una variable global que se imprime al final del programa.

## 2. Resultados

En escala log-log, una función c·nᵏ es una recta de pendiente k. Por eso la pendiente de cada curva en los tamaños grandes estima el exponente real del algoritmo. Ajustando una recta por mínimos cuadrados para n ≥ 256 se obtuvo un exponente de [≈ 2,0] para la multiplicación clásica y de [≈ 1,6] para Karatsuba, coherentes con los valores teóricos 2 y log₂3 ≈ 1,585.

## 3. Interpretación del punto de cruce

[Si aparece el cruce:] Karatsuba resultó más rápido a partir de n = [X] dígitos y mantuvo la ventaja en todos los tamaños mayores. Para n = 4096 fue [Y] veces más rápido que el método clásico. La razón de tiempos disminuye de forma sostenida al crecer n, como predice el análisis asintótico: el cociente entre n^1.585 y n² es n^(−0.415), que tiende a cero.

[Si no aparece el cruce:] En el rango medido el método clásico fue siempre más rápido, aunque la razón de tiempos disminuyó de [A] a [B] al pasar de [n₁] a 4096 dígitos. La ventaja asintótica de Karatsuba sí se observa en la pendiente (exponente empírico ≈ [1,6] frente a ≈ 2), pero su constante es tan grande que el cruce queda fuera del rango. Extrapolando, ocurriría alrededor de n ≈ [Z].

**¿Por qué el método clásico es más rápido con números pequeños?** La notación Θ describe cómo crece el tiempo, pero oculta las constantes multiplicativas y los términos de menor orden, que son los que deciden con números pequeños:

- **El clásico hace un trabajo mínimo por paso.** Su ciclo interno es una multiplicación de dígitos, una suma y un acceso a un arreglo contiguo, sin reservar memoria. Esto aprovecha bien la caché y el compilador lo optimiza muy bien.
- **Karatsuba paga un costo fijo en cada llamada recursiva.** Cada nivel crea cadenas nuevas: el relleno con ceros, las dos mitades de `partir`, los resultados de `sumar`, `restar` y `desplazar` y la normalización. Cada cadena implica una asignación de memoria dinámica y copias de caracteres. Para n pequeño ese costo fijo es mucho mayor que las pocas multiplicaciones de dígitos que se ahorran.
- **Llamadas recursivas.** Con caso base de 1 dígito, multiplicar dos números de n dígitos genera del orden de n^log₂3 llamadas. Por ejemplo, ≈ 3⁸ = 6561 para n = 256, cada una con su propia sobrecarga de llamada y de memoria.
- **El término Θ(n) no es despreciable.** Las sumas, restas y recomposiciones se hacen en cada nivel, y su costo real depende de cómo estén implementadas. Si una operación auxiliar fuera cuadrática en vez de lineal, la recurrencia dejaría de ser T(n) = 3T(n/2) + Θ(n) y el exponente observado se acercaría a 2.

**Conclusión.** La complejidad asintótica indica qué algoritmo gana cuando n es suficientemente grande, pero no a partir de qué tamaño. Ese umbral depende de las constantes de la implementación: representación de los números, copias, asignaciones de memoria y caso base.

## 4. uso ético de IA

