/*
 * Descripción:
 * Contiene las declaraciones de las operaciones auxiliares
 * para trabajar con números enteros de longitud arbitraria,
 * representados mediante cadenas de caracteres en base 10.
 *
*/

#ifndef OPERACIONES_H
#define OPERACIONES_H

#include <string>
#include <utility>

using namespace std;

// Elimina los 0 innecesarios a la izquierda, pero si es cero devuelve "0".
string normalizar(const string& A);

// Recibe 2 números representados como cadenas y devuelve su suma, usa "carry".
string sumar(const string& A, const string& B);

// Recibe 2 números representados como cadenas y devuelve su resta, usa "borrow".
string restar(const string& A, const string& B);

// Recibe un número A y agrega 'k' ceros a la derecha de la cadena.
string desplazar(const string& A, int k);

// Recibe una cadena A y un entero 'm', devuelve dos cadenas: mitad alta y mitad baja.
pair<string, string> partir(const string& A, int m);

#endif