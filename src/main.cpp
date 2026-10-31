/*
 * Descripción:
 * Programa principal que valida la correctitud de Karatsuba
 * frente a la multiplicación clásica según los casos exigidos.
 */

#include <iostream>
#include <vector>
#include <string>
#include "multiplicacion_clasica.h"
#include "karatsuba.h"

using namespace std;

void validar(const string& caso, const string& A, const string& B) {
    string resClasica = multiplicacionClasica(A, B);
    string resKaratsuba = karatsuba(A, B);
    
    cout << "Caso: " << caso << "\n";
    cout << "A: " << A << " | B: " << B << "\n";
    if (resClasica == resKaratsuba) {
        cout << "[EXITO] Ambos métodos retornan: " << resClasica << "\n\n";
    } else {
        cout << "[ERROR] Clásica: " << resClasica << " | Karatsuba: " << resKaratsuba << "\n\n";
    }
}

int main() {
    cout << "VALIDACION DE CORRECTITUD \n\n";

    validar("Números de 1 dígito", "7", "8");
    validar("Muchos ceros a la izquierda", "00000123", "000456");
    validar("Ceros intercalados", "10002", "30004");
    validar("Longitudes impares antes del relleno", "12345", "678");
    
    // Producto grande (varios cientos de dígitos)
    string numGrande1 = string(100, '9'); 
    string numGrande2 = string(100, '9');
    validar("Producto grande (100 dígitos)", numGrande1, numGrande2);

    return 0;
}