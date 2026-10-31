/*
 * Descripción:
 * Implementación del algoritmo de Karatsuba aplicando Divide y Vencerás.
 * Complejidad temporal: O(n^1.585)
 */

#include "karatsuba.h"
#include "multiplicacion_clasica.h"
#include "operaciones.h"
#include <algorithm>

// Función auxiliar para encontrar la siguiente potencia de 2
int siguientePotenciaDe2(int n) {
    int pot = 1;
    while (pot < n) {
        pot *= 2;
    }
    return pot;
}

string karatsuba(const string& A, const string& B) {
    // Normalizar para evitar ceros sobrantes en la evaluación del caso base
    string numA = normalizar(A);
    string numB = normalizar(B);

    // Caso base: 1 dígito o si alguno es "0"
    if (numA == "0" || numB == "0") return "0";
    if (numA.size() <= 1 && numB.size() <= 1) {
        return multiplicacionClasica(numA, numB);
    }

    // Igualar longitudes a la misma potencia de 2 mediante relleno de ceros a la izquierda
    int maxLen = max(numA.size(), numB.size());
    int n = siguientePotenciaDe2(maxLen);

    if (numA.size() < n) numA = string(n - numA.size(), '0') + numA;
    if (numB.size() < n) numB = string(n - numB.size(), '0') + numB;

    int m = n / 2;

    // Partir en mitades alta y baja respecto a m
    pair<string, string> partesA = partir(numA, m);
    pair<string, string> partesB = partir(numB, m);

    string x1 = partesA.first;
    string x0 = partesA.second;
    string y1 = partesB.first;
    string y0 = partesB.second;

    // Tres llamadas recursivas
    string P1 = karatsuba(x1, y1);
    string P2 = karatsuba(x0, y0);
    
    string sumaA = sumar(x1, x0);
    string sumaB = sumar(y1, y0);
    string P3 = karatsuba(sumaA, sumaB);

    // Calcular el término central respetando el orden para evitar negativos intermedios: (P3 - P1) - P2
    string resta1 = restar(P3, P1);
    string terminoCentral = restar(resta1, P2);

    // Ensamblar el resultado: P1 * 10^(2m) + terminoCentral * 10^m + P2
    string term1 = desplazar(P1, 2 * m);
    string term2 = desplazar(terminoCentral, m);

    string resultado = sumar(sumar(term1, term2), P2);

    return normalizar(resultado);
}