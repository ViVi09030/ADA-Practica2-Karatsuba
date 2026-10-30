# Decisiones de implementación

Este documento registra las decisiones técnicas tomadas durante el desarrollo de la Práctica 2, correspondiente a la implementación de la multiplicación de números enteros mediante el algoritmo clásico y el algoritmo de Karatsuba.

## 1. Representación de los números

Se decidió representar los números enteros mediante cadenas de caracteres (`std::string`) en base 10.

Esta representación permite trabajar con números de longitud arbitraria sin convertir los operandos completos a tipos numéricos nativos, evitando así las limitaciones de tamaño de los tipos enteros convencionales.

Los dígitos se almacenan de izquierda a derecha, donde el primer carácter representa el dígito más significativo. Para realizar las operaciones aritméticas, cada carácter se convierte temporalmente a su valor numérico mediante la operación `digito - '0'`.

Se establecieron las siguientes convenciones:

- Los números no conservan ceros innecesarios a la izquierda.
- El número cero se representa mediante la cadena `"0"`.
- Los operandos se manejan como números enteros no negativos.

## 2. Operaciones auxiliares

Se implementaron funciones independientes para facilitar las operaciones requeridas por los algoritmos de multiplicación.

- **Normalización (`normalizar`):** elimina los ceros iniciales de una cadena y garantiza que el cero se represente como `"0"`.
- **Suma (`sumar`):** recorre las cadenas de derecha a izquierda, sumando los dígitos correspondientes y utilizando un acarreo cuando el resultado supera 9. Permite que el resultado tenga un dígito adicional.
- **Resta (`restar`):** realiza la operación de derecha a izquierda mediante préstamos entre posiciones. Se establece como precondición que el primer operando sea mayor o igual que el segundo, por lo que no se requiere manejar resultados negativos.
- **Desplazamiento (`desplazar`):** agrega ceros al final de la cadena para representar una multiplicación por una potencia de 10.
- **División en partes (`partir`):** separa una cadena en dos segmentos, correspondientes a la parte alta y la parte baja del número, según una posición de división indicada.

Estas funciones se diseñaron para trabajar directamente con cadenas y evitar la conversión de los números completos a tipos numéricos nativos.

## 3. Multiplicación clásica

Se implementó la multiplicación clásica mediante dos ciclos anidados que recorren los dígitos de ambos operandos de derecha a izquierda.

Para almacenar los resultados parciales se utiliza un vector de enteros de tamaño `n + m`, donde `n` y `m` corresponden a las longitudes de los operandos.

En cada iteración se multiplica un par de dígitos, se suma el valor previamente almacenado en la posición correspondiente y se incorpora el acarreo de la operación anterior. El resultado se distribuye entre el dígito correspondiente y el nuevo acarreo.

Finalmente, los dígitos almacenados en el vector se convierten nuevamente en una cadena y se normalizan.

## 4. Manejo de memoria

Se utilizaron referencias constantes (`const string&`) para recibir los operandos en las funciones, evitando realizar copias innecesarias de las cadenas de entrada.

Las funciones que generan resultados devuelven nuevas cadenas, por lo que los operandos originales no se modifican.

Para la multiplicación clásica se utiliza un vector auxiliar de tamaño `n + m`, inicializado en cero, que almacena los dígitos del producto.

## 5. Iteraciones y complejidad

Las operaciones auxiliares recorren los dígitos de las cadenas para realizar sus respectivos cálculos:

- La normalización tiene complejidad lineal respecto a la longitud de la cadena.
- La suma y la resta tienen complejidad lineal respecto a la longitud de los operandos.
- El desplazamiento tiene complejidad lineal respecto al tamaño del resultado generado.
- La división en partes requiere tiempo lineal respecto a la longitud de la cadena.

En la multiplicación clásica se utilizan dos ciclos anidados, uno por cada operando. Por tanto, si los operandos tienen longitudes `n` y `m`, su complejidad temporal es Θ(nm). Cuando ambos tienen la misma longitud, la complejidad es Θ(n²).

El espacio auxiliar utilizado por la multiplicación clásica es Θ(n + m), correspondiente al vector que almacena el resultado.

## 6. Consideraciones generales

Se optó por separar las operaciones auxiliares de los algoritmos de multiplicación para favorecer la modularidad, reutilización y mantenimiento del código.

La implementación utiliza C++17 y evita almacenar los operandos completos en tipos numéricos nativos, de acuerdo con las condiciones establecidas para la práctica.