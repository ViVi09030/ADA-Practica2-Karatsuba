#include "karatsuba.h"
#include "multiplicacion_clasica.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

const int PARES = 5;
const int MEDICIONES = 5;
const double TIEMPO_MINIMO = 0.1;

unsigned long long sumidero = 0;
mt19937 generador(2026);

string numeroAleatorio(int n) {
    uniform_int_distribution<int> digito(0, 9), primero(1, 9);
    string s(1, char('0' + primero(generador)));
    for (int i = 1; i < n; i++) s += char('0' + digito(generador));
    return s;
}

double ejecutar(string (*algoritmo)(const string&, const string&),
                const vector<pair<string, string>>& pares, long long repeticiones) {
    auto inicio = chrono::steady_clock::now();
    for (long long r = 0; r < repeticiones; r++) {
        for (const auto& [A, B] : pares) {
            string resultado = algoritmo(A, B);
            // El resultado se consume para que el compilador no elimine el calculo
            sumidero += resultado.size() + resultado.back();
        }
    }
    return chrono::duration<double>(chrono::steady_clock::now() - inicio).count();
}

// Tiempo promedio por multiplicacion en microsegundos. Las repeticiones se duplican
// hasta que cada medicion supere TIEMPO_MINIMO, muy por encima de la resolucion del reloj.
double medir(string (*algoritmo)(const string&, const string&),
             const vector<pair<string, string>>& pares) {
    long long repeticiones = 1;
    while (ejecutar(algoritmo, pares, repeticiones) < TIEMPO_MINIMO) repeticiones *= 2;

    double total = 0;
    for (int i = 0; i < MEDICIONES; i++) total += ejecutar(algoritmo, pares, repeticiones);
    return total / (MEDICIONES * repeticiones * pares.size()) * 1e6;
}

int main() {
#ifdef RAIZ_PROYECTO
    string carpeta = string(RAIZ_PROYECTO) + "/resultados";
#else
    string carpeta = "resultados";
#endif
    filesystem::create_directories(carpeta);
    ofstream csv(carpeta + "/tiempos.csv");
    csv << "n,clasica_us,karatsuba_us\n";

    cout << "n\tclasica_us\tkaratsuba_us\n";
    int cruce = -1;
    for (int n = 2; n <= 4096; n *= 2) {
        vector<pair<string, string>> pares;
        for (int i = 0; i < PARES; i++) pares.push_back({numeroAleatorio(n), numeroAleatorio(n)});

        for (const auto& [A, B] : pares) {
            if (karatsuba(A, B) != multiplicacionClasica(A, B)) {
                cout << "Error: los algoritmos no coinciden para n = " << n << "\n";
                return 1;
            }
        }

        double tc = medir(multiplicacionClasica, pares);
        double tk = medir(karatsuba, pares);
        cout << n << "\t" << tc << "\t" << tk << "\n";
        csv << n << "," << tc << "," << tk << "\n";

        // Punto de cruce: menor n desde el cual Karatsuba gana en todos los tamanos siguientes
        if (tk >= tc) cruce = -1;
        else if (cruce == -1) cruce = n;
    }

    if (cruce == -1) cout << "Punto de cruce: no aparece entre 2 y 4096 digitos\n";
    else cout << "Punto de cruce: n = " << cruce << "\n";
    cout << "Sumidero: " << sumidero << "\n";
    return 0;
}
