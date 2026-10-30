/*
 * Descripcion:
 * Implementa el algoritmo de multiplicacion clasica
 * mediante operaciones sobre digitos individuales.
 *
 * Complejidad temporal: O(n^2).
*/

#include "multiplicacion_clasica.h"
#include "operaciones.h"
#include <vector>

string multiplicacionClasica(const string& A, const string& B) {

    // Elimina ceros innecesarios de los operandos
    string numA = normalizar(A);
    string numB = normalizar(B);

    // Si alguno de los operandos es 0, el resultado es 0
    if (numA == "0" || numB == "0") {
        return "0";
    }

    int n = numA.size();
    int m = numB.size();

    // Reserva espacio para el resultado
    vector<int> resultado(n + m, 0);

    // Recorre los digitos de A: desde el último hasta el primero
    for (int i = n - 1; i >= 0; i--) {

        int acarreo = 0;

        // Recorre los digitos de B de la misma manera
        for (int j = m - 1; j >= 0; j--) {

            // Suma el producto de los 2 digitos actuales, el valor ya almacenado en la posición
            // y el acarreo de la multiplicación anterior.
            int producto = (numA[i] - '0') * (numB[j] - '0') + resultado[i + j + 1] + acarreo;

            resultado[i + j + 1] = producto % 10;
            acarreo = producto / 10;
        }

        resultado[i] += acarreo;
    }

    // Convierte el vector de digitos a una cadena
    string producto = "";

    for (int digito : resultado) {
        producto += char('0' + digito);
    }

    return normalizar(producto);
}