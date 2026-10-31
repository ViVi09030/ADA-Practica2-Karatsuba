
#include "karatsuba.h"
#include "multiplicacion_clasica.h"
#include "operaciones.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

const int TAMANO_MINIMO = 2;
const int TAMANO_MAXIMO = 4096;
const int PARES_POR_TAMANO = 5;
const int MEDICIONES = 5;
const double TIEMPO_MINIMO_SEG = 0.1;

unsigned long long sumidero = 0;

mt19937 generador(2026);

struct Medicion {
    double promedioUs;
    double desviacionUs;
    long long repeticiones;
};

struct Fila {
    int n;
    Medicion clasica;
    Medicion karatsuba;
    bool correcto;
};


string numeroAleatorio(int n) {
    uniform_int_distribution<int> primerDigito(1, 9);
    uniform_int_distribution<int> digito(0, 9);

    string s(1, char('0' + primerDigito(generador)));
    for (int i = 1; i < n; i++) {
        s += char('0' + digito(generador));
    }
    return s;
}

template <typename Algoritmo>
double ejecutarLote(Algoritmo algoritmo, const vector<pair<string, string>>& pares,
                    long long repeticiones) {
    auto inicio = chrono::steady_clock::now();

    for (long long r = 0; r < repeticiones; r++) {
        for (const auto& par : pares) {
            string resultado = algoritmo(par.first, par.second);
            // El resultado se consume para que el compilador no elimine el calculo
            sumidero += resultado.size() + (resultado.back() - '0');
        }
    }

    auto fin = chrono::steady_clock::now();
    return chrono::duration<double>(fin - inicio).count();
}

// Repeticiones suficientes para que el tiempo medido supere con holgura la resolucion del reloj
template <typename Algoritmo>
Medicion medir(Algoritmo algoritmo, const vector<pair<string, string>>& pares) {
    ejecutarLote(algoritmo, pares, 1);

    long long repeticiones = 1;
    while (ejecutarLote(algoritmo, pares, repeticiones) < TIEMPO_MINIMO_SEG) {
        repeticiones *= 2;
    }

    vector<double> tiemposUs;
    for (int i = 0; i < MEDICIONES; i++) {
        double segundos = ejecutarLote(algoritmo, pares, repeticiones);
        double multiplicaciones = double(repeticiones) * pares.size();
        tiemposUs.push_back(segundos / multiplicaciones * 1e6);
    }

    double suma = 0;
    for (double t : tiemposUs) suma += t;
    double promedio = suma / tiemposUs.size();

    double sumaCuadrados = 0;
    for (double t : tiemposUs) sumaCuadrados += (t - promedio) * (t - promedio);
    double desviacion = sqrt(sumaCuadrados / (tiemposUs.size() - 1));

    return {promedio, desviacion, repeticiones};
}

bool guardarCsv(const string& ruta, const vector<Fila>& filas) {
    filesystem::path carpeta = filesystem::path(ruta).parent_path();
    if (!carpeta.empty()) {
        error_code ec;
        filesystem::create_directories(carpeta, ec);
    }

    ofstream archivo(ruta);
    if (!archivo) {
        return false;
    }

    archivo << "n,clasica_us,clasica_desv_us,karatsuba_us,karatsuba_desv_us,"
               "razon_karatsuba_clasica,repeticiones_clasica,repeticiones_karatsuba,"
               "pares,correcto\n";
    archivo << fixed << setprecision(4);
    for (const Fila& f : filas) {
        archivo << f.n << ","
                << f.clasica.promedioUs << "," << f.clasica.desviacionUs << ","
                << f.karatsuba.promedioUs << "," << f.karatsuba.desviacionUs << ","
                << f.karatsuba.promedioUs / f.clasica.promedioUs << ","
                << f.clasica.repeticiones << "," << f.karatsuba.repeticiones << ","
                << PARES_POR_TAMANO << ","
                << (f.correcto ? "si" : "no") << "\n";
    }
    return true;
}


int main(int argc, char* argv[]) {
#ifdef RAIZ_PROYECTO
    string rutaPorDefecto = string(RAIZ_PROYECTO) + "/resultados/tiempos.csv";
#else
    string rutaPorDefecto = "resultados/tiempos.csv";
#endif
    string rutaCsv = (argc > 1) ? argv[1] : rutaPorDefecto;

#ifndef __OPTIMIZE__
    cout << "AVISO: el programa se compilo sin optimizacion. "
            "Compile con -O2 para obtener tiempos representativos.\n\n";
#endif

    auto clasica = [](const string& A, const string& B) { return multiplicacionClasica(A, B); };
    auto kara = [](const string& A, const string& B) { return karatsuba(A, B); };

    cout << "=== Experimento: multiplicacion clasica vs. Karatsuba ===\n";
    cout << "Pares por tamano: " << PARES_POR_TAMANO << " | Mediciones: " << MEDICIONES
         << " | Tiempo minimo por medicion: " << TIEMPO_MINIMO_SEG << " s\n";
    cout << "Tiempos en microsegundos por multiplicacion (promedio +- desviacion)\n\n";

    cout << setw(6) << "n" << setw(26) << "Clasica (us)" << setw(26) << "Karatsuba (us)"
         << setw(10) << "K / C" << "  Gana\n";
    cout << string(76, '-') << "\n";

    vector<Fila> filas;

    for (int n = TAMANO_MINIMO; n <= TAMANO_MAXIMO; n *= 2) {
        vector<pair<string, string>> pares;
        for (int i = 0; i < PARES_POR_TAMANO; i++) {
            pares.push_back({numeroAleatorio(n), numeroAleatorio(n)});
        }

        bool correcto = true;
        for (const auto& par : pares) {
            if (karatsuba(par.first, par.second) != multiplicacionClasica(par.first, par.second)) {
                correcto = false;
            }
        }
        if (!correcto) {
            cout << "ERROR: para n = " << n << " Karatsuba y la clasica no coinciden. "
                 << "Ejecute las pruebas (build/pruebas).\n";
        }

        Fila fila{n, medir(clasica, pares), medir(kara, pares), correcto};
        filas.push_back(fila);

        double razon = fila.karatsuba.promedioUs / fila.clasica.promedioUs;
        cout << fixed << setprecision(3)
             << setw(6) << n
             << setw(14) << fila.clasica.promedioUs << " +- " << setw(8) << fila.clasica.desviacionUs
             << setw(14) << fila.karatsuba.promedioUs << " +- " << setw(8) << fila.karatsuba.desviacionUs
             << setw(10) << setprecision(2) << razon
             << "  " << (razon < 1 ? "Karatsuba" : "Clasica") << "\n";
    }

    // Punto de cruce: menor n a partir del cual Karatsuba gana en todos los tamanos siguientes
    int cruce = -1;
    for (int i = (int)filas.size() - 1; i >= 0; i--) {
        if (filas[i].karatsuba.promedioUs < filas[i].clasica.promedioUs) {
            cruce = filas[i].n;
        } else {
            break;
        }
    }

    cout << "\n";
    if (cruce == -1) {
        cout << "Punto de cruce: no aparece en el rango medido (" << TAMANO_MINIMO << " a "
             << TAMANO_MAXIMO << " digitos). La clasica fue mas rapida en el tamano maximo.\n";
    } else {
        cout << "Punto de cruce: a partir de n = " << cruce
             << " digitos Karatsuba es mas rapido en todos los tamanos medidos.\n";
    }

    if (guardarCsv(rutaCsv, filas)) {
        cout << "Resultados guardados en: " << rutaCsv << "\n";
    } else {
        cout << "No se pudo escribir el archivo " << rutaCsv << "\n";
    }

    cout << "(verificacion del sumidero: " << sumidero << ")\n";
    return 0;
}
