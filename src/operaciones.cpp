/*
 * Descripción:
 * Implementa las operaciones auxiliares utilizadas por
 * los algoritmos de multiplicación.
 *
 * Funciones:
 * - normalizar(): elimina ceros innecesarios a la izquierda.
 * - sumar(): suma dos números representados como cadenas.
 * - restar(): resta dos números cuando A >= B.
 * - desplazar(): agrega ceros a la derecha.
 * - partir(): divide un número en mitad alta y baja.
*/

#include "operaciones.h"
#include <algorithm>

string normalizar(const string& A) {
    // Busca el primer caracter diferente de 0
    size_t posicion = A.find_first_not_of('0');

    // Si todos los caracteres son cero, devuelve "0"
    if (posicion == string::npos) {
        return "0";
    }

    return A.substr(posicion);
}


string sumar(const string& A, const string& B) {
    
    int i = A.size() - 1;
    int j = B.size() - 1;

    // Almacena el valor que se debe llevar a la siguiente posición, como se hace a mano.
    int acarreo = 0;

    string resultado = "";

    // El ciclo no termina mientras exista un acarreo pendiente
    while (i >= 0 || j >= 0 || acarreo > 0) {

        int suma = acarreo;

        if (i >= 0) {
            suma += A[i] - '0';
            i--;
        }

        if (j >= 0) {
            suma += B[j] - '0';
            j--;
        }

        // Se usa "% 10" para obtener el dígito que se debe guardar
        resultado.push_back(char('0' + suma % 10));

        // Se usa "/ 10" para obtener el acarreo
        acarreo = suma / 10;
    }

    // Los digitos se agregaron del menos al mas significativo
    reverse(resultado.begin(), resultado.end());

    return normalizar(resultado);
}


string restar(const string& A, const string& B) {

    int i = A.size() - 1;
    int j = B.size() - 1;

    int prestamo = 0;

    string resultado = "";

    while (i >= 0) {
        
        // Se convierte el caracter en entero y se descuenta el prestamo anterior.
        // Obtiene los dígitos
        int digitoA = (A[i] - '0') - prestamo;
        int digitoB = 0;

        if (j >= 0) {
            digitoB = B[j] - '0';
        }
        
        // Comprobar si es necesario pedir prestado
        if (digitoA < digitoB) {
            digitoA += 10;
            prestamo = 1;
        } else {
            prestamo = 0;
        }

        // Calcula la diferencia
        int diferencia = digitoA - digitoB;

        resultado.push_back(char('0' + diferencia));

        i--;
        j--;
    }

    // Los digitos se agregaron del menos al mas significativo
    reverse(resultado.begin(), resultado.end());

    return normalizar(resultado);
}


string desplazar(const string& A, int k) {

    // Devuelve el numero original
    if (k <= 0) {
        return A;
    }

    // Crea cadena con k repeticiones de '0' y se concatena a A
    return A + string(k, '0');
}


pair<string, string> partir(const string& A, int m) {
    
    // Obtiene la cantidad de dígitos de A
    int longitud = A.size();

    // Extrae desde el índice 0, hasta antes de los últimos m dígitos
    string alta = A.substr(0, longitud - m);
    // Extrae los últimos m dígitos
    string baja = A.substr(longitud - m, m);

    return {normalizar(alta), normalizar(baja)};
}