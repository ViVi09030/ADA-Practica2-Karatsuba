#include "karatsuba.h"
#include "multiplicacion_clasica.h"

#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

mt19937 generador(2026);
int casos = 0, fallos = 0;

int aleatorio(int a, int b) {
    return uniform_int_distribution<int>(a, b)(generador);
}

// Numero de n digitos sin cero inicial; cada digito siguiente es 0 con probabilidad pCero
string numero(int n, double pCero = 0.1) {
    string s(1, char('0' + aleatorio(1, 9)));
    for (int i = 1; i < n; i++) {
        bool cero = uniform_real_distribution<double>(0, 1)(generador) < pCero;
        s += cero ? '0' : char('0' + aleatorio(1, 9));
    }
    return s;
}

// Compara karatsuba contra multiplicacionClasica y, si se da, contra el valor exacto
void probar(const string& A, const string& B, const string& esperado = "") {
    casos++;
    string c = multiplicacionClasica(A, B);
    string k = karatsuba(A, B);
    if (k != c || (!esperado.empty() && c != esperado)) {
        fallos++;
        cout << "Fallo con A de " << A.size() << " digitos y B de " << B.size() << " digitos\n";
    }
}

void reportar(const string& nombre, int casosAntes, int fallosAntes) {
    cout << nombre << ": " << (casos - casosAntes) - (fallos - fallosAntes) << "/"
         << casos - casosAntes << "\n";
}

int main() {
    int c0 = casos, f0 = fallos;
    probar("0", "0", "0");
    probar("0", "123456789", "0");
    probar("12345678", "87654321", "1082152022374638");
    probar("111111111", "111111111", "12345678987654321");
    probar("99999999", "99999999", "9999999800000001");
    for (int a : {0, 1, 7, 64, 100})
        for (int b : {0, 3, 50})
            probar("1" + string(a, '0'), "1" + string(b, '0'), "1" + string(a + b, '0'));
    for (int n : {1, 2, 5, 16, 33, 128})
        probar("1" + string(n - 1, '0') + "1", string(n, '9'), string(2 * n, '9'));
    reportar("Valores conocidos", c0, f0);

    // Caso exigido: numeros de 1 digito
    c0 = casos; f0 = fallos;
    for (int i = 0; i <= 9; i++)
        for (int j = 0; j <= 9; j++)
            probar(to_string(i), to_string(j), to_string(i * j));
    reportar("1 digito", c0, f0);

    // Caso exigido: operandos con muchos ceros a la izquierda
    c0 = casos; f0 = fallos;
    probar("0005", "3", "15");
    probar("000000000123", "000456", "56088");
    probar("0000", "999", "0");
    probar(string(100, '0') + "7", "8", "56");
    probar(string(63, '0') + "12345678", string(5, '0') + "87654321", "1082152022374638");
    for (int k : {1, 5, 37, 200}) {
        for (int r = 0; r < 10; r++) {
            string A = string(k, '0') + numero(aleatorio(1, 60));
            string B = numero(aleatorio(1, 60));
            probar(A, B);
            probar(B, A);
        }
    }
    reportar("Ceros a la izquierda", c0, f0);

    // Caso exigido: factores con ceros intercalados
    c0 = casos; f0 = fallos;
    probar("1001", "1001", "1002001");
    probar("10203", "40506", "413282718");
    probar("12340000", "56780000", "700665200000000");
    for (int n : {2, 3, 9, 64, 257}) {
        string x = "1" + string(n - 2, '0') + "1";
        probar(x, x, "1" + string(n - 2, '0') + "2" + string(n - 2, '0') + "1");
    }
    for (int r = 0; r < 60; r++)
        probar(numero(aleatorio(2, 200), 0.7), numero(aleatorio(2, 200), 0.7));
    reportar("Ceros intercalados", c0, f0);

    // Caso exigido: longitudes impares antes del relleno
    c0 = casos; f0 = fallos;
    probar("123", "45678", "5618394");
    probar("99999", "999", "99899001");
    vector<pair<int, int>> longitudes = {{3, 3}, {5, 7}, {1, 9}, {9, 1}, {7, 2}, {13, 11},
                                         {33, 65}, {127, 129}, {255, 3}, {1, 513}, {999, 1001}};
    for (auto [n, m] : longitudes)
        for (int r = 0; r < 3; r++)
            probar(numero(n), numero(m));
    reportar("Longitudes impares", c0, f0);

    // x1 + x0 con un digito extra: P3 recibe operandos de m + 1 digitos
    c0 = casos; f0 = fallos;
    for (int n : {2, 3, 4, 5, 7, 8, 15, 16, 31, 64, 100, 256, 512})
        probar(string(n, '9'), string(n, '9'), string(n - 1, '9') + "8" + string(n - 1, '0') + "1");
    reportar("Acarreo en x1 + x0", c0, f0);

    // Caso exigido: al menos un producto grande (varios cientos de digitos)
    c0 = casos; f0 = fallos;
    for (int n : {300, 512, 777, 1000, 2048, 4096})
        probar(numero(n), numero(n));
    probar(numero(1000), numero(17));
    probar("1" + string(599, '0') + "1", string(600, '9'), string(1200, '9'));
    reportar("Productos grandes", c0, f0);

    c0 = casos; f0 = fallos;
    for (int r = 0; r < 1000; r++) {
        string A = numero(aleatorio(1, 256));
        if (aleatorio(1, 5) == 1) A = string(aleatorio(1, 20), '0') + A;
        probar(A, numero(aleatorio(1, 256)));
    }
    reportar("Aleatorios", c0, f0);

    cout << "Total: " << casos - fallos << "/" << casos << "\n";
    return fallos == 0 ? 0 : 1;
}
