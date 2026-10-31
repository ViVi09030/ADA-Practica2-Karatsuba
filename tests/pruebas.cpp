

#include "karatsuba.h"
#include "multiplicacion_clasica.h"
#include "operaciones.h"

#include <exception>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

mt19937 generador(2026);

int totalCasos = 0;
int totalFallos = 0;
int categoriasFallidas = 0;

struct Categoria {
    string nombre;
    int casos = 0;
    int fallos = 0;
};


string abreviar(const string& s) {
    if (s.size() <= 40) {
        return s;
    }
    return s.substr(0, 15) + "..." + s.substr(s.size() - 15) +
           " (" + to_string(s.size()) + " digitos)";
}

string numeroAleatorio(int n) {
    uniform_int_distribution<int> primerDigito(1, 9);
    uniform_int_distribution<int> digito(0, 9);

    string s(1, char('0' + primerDigito(generador)));
    for (int i = 1; i < n; i++) {
        s += char('0' + digito(generador));
    }
    return s;
}

string numeroConCeros(int n, double pCero) {
    bernoulli_distribution esCero(pCero);
    uniform_int_distribution<int> digitoNoCero(1, 9);

    string s(1, char('0' + digitoNoCero(generador)));
    for (int i = 1; i < n; i++) {
        s += esCero(generador) ? '0' : char('0' + digitoNoCero(generador));
    }
    return s;
}

int enteroAleatorio(int a, int b) {
    uniform_int_distribution<int> d(a, b);
    return d(generador);
}


// Compara karatsuba contra multiplicacionClasica y, si se conoce, contra el valor exacto
void probar(Categoria& cat, const string& A, const string& B, const string& esperado = "") {
    cat.casos++;

    string clasica, kara, motivo = "";
    try {
        clasica = multiplicacionClasica(A, B);
    } catch (const exception& e) {
        motivo = string("la clasica lanzo una excepcion: ") + e.what();
    }
    try {
        kara = karatsuba(A, B);
    } catch (const exception& e) {
        if (motivo.empty()) {
            motivo = string("karatsuba lanzo una excepcion: ") + e.what();
        }
    }

    if (motivo.empty()) {
        if (!esperado.empty() && clasica != esperado && kara != esperado) {
            motivo = "ningun algoritmo da el valor esperado";
        } else if (!esperado.empty() && clasica != esperado) {
            motivo = "la clasica (referencia) no da el valor esperado";
        } else if (normalizar(kara) != clasica) {
            motivo = "karatsuba no coincide con la clasica";
        } else if (kara != clasica) {
            motivo = "karatsuba da el valor correcto pero con ceros a la izquierda";
        }
    }

    if (!motivo.empty()) {
        cat.fallos++;
        if (cat.fallos <= 3) {
            cout << "    FALLO: " << motivo << "\n";
            cout << "      A         = " << abreviar(A) << "\n";
            cout << "      B         = " << abreviar(B) << "\n";
            cout << "      clasica   = " << abreviar(clasica) << "\n";
            cout << "      karatsuba = " << abreviar(kara) << "\n";
            if (!esperado.empty()) {
                cout << "      esperado  = " << abreviar(esperado) << "\n";
            }
        }
    }
}

void reportar(const Categoria& cat) {
    totalCasos += cat.casos;
    totalFallos += cat.fallos;

    if (cat.fallos == 0) {
        cout << "[ OK ]  " << cat.nombre << ": " << cat.casos << "/" << cat.casos << " casos\n";
    } else {
        categoriasFallidas++;
        cout << "[FALLA] " << cat.nombre << ": " << cat.fallos << " de " << cat.casos
             << " casos fallaron\n";
    }
}


void pruebasReferencia() {
    Categoria cat{"Referencia: productos de valor conocido"};

    probar(cat, "0", "0", "0");
    probar(cat, "0", "123456789", "0");
    probar(cat, "1", "1", "1");
    probar(cat, "12345678", "87654321", "1082152022374638");
    probar(cat, "111111111", "111111111", "12345678987654321");
    probar(cat, "99999999", "99999999", "9999999800000001");

    for (int a : {0, 1, 7, 64, 100}) {
        for (int b : {0, 3, 50}) {
            probar(cat, "1" + string(a, '0'), "1" + string(b, '0'), "1" + string(a + b, '0'));
        }
    }

    for (int n : {1, 2, 5, 16, 33, 128}) {
        string mas1 = "1" + string(n - 1, '0') + "1";
        string menos1 = string(n, '9');
        probar(cat, mas1, menos1, string(2 * n, '9'));
    }

    reportar(cat);
}

// Caso exigido: numeros de 1 digito
void pruebasUnDigito() {
    Categoria cat{"Numeros de 1 digito (todas las combinaciones 0-9)"};

    for (int i = 0; i <= 9; i++) {
        for (int j = 0; j <= 9; j++) {
            probar(cat, to_string(i), to_string(j), to_string(i * j));
        }
    }

    reportar(cat);
}

// Caso exigido: operandos con muchos ceros a la izquierda
void pruebasCerosIzquierda() {
    Categoria cat{"Operandos con muchos ceros a la izquierda"};

    probar(cat, "0005", "3", "15");
    probar(cat, "0001", "0001", "1");
    probar(cat, "000000000123", "000456", "56088");
    probar(cat, "0000", "999", "0");
    probar(cat, "000000", "0000000", "0");
    probar(cat, string(100, '0') + "7", "8", "56");
    probar(cat, string(63, '0') + "12345678", string(5, '0') + "87654321", "1082152022374638");

    for (int k : {1, 5, 37, 200}) {
        for (int rep = 0; rep < 10; rep++) {
            string A = string(k, '0') + numeroAleatorio(enteroAleatorio(1, 60));
            string B = numeroAleatorio(enteroAleatorio(1, 60));
            probar(cat, A, B);
            probar(cat, B, A);
        }
    }

    reportar(cat);
}

// Caso exigido: factores con ceros intercalados
void pruebasCerosIntercalados() {
    Categoria cat{"Factores con ceros intercalados"};

    probar(cat, "1001", "1001", "1002001");
    probar(cat, "10203", "40506", "413282718");
    probar(cat, "100000001", "100000001", "10000000200000001");
    probar(cat, "5000005", "20", "100000100");
    probar(cat, "1000000", "1000000", "1000000000000");
    probar(cat, "12340000", "56780000", "700665200000000");

    for (int n : {2, 3, 9, 64, 257}) {
        string x = "1" + string(n - 2, '0') + "1";
        string esperado = "1" + string(n - 2, '0') + "2" + string(n - 2, '0') + "1";
        probar(cat, x, x, esperado);
    }

    for (int rep = 0; rep < 60; rep++) {
        probar(cat, numeroConCeros(enteroAleatorio(2, 200), 0.7),
                    numeroConCeros(enteroAleatorio(2, 200), 0.7));
    }

    reportar(cat);
}

// Caso exigido: longitudes impares antes del relleno
void pruebasLongitudesImpares() {
    Categoria cat{"Longitudes impares antes del relleno"};

    probar(cat, "123", "45678", "5618394");
    probar(cat, "99999", "999", "99899001");
    probar(cat, "12345", "6789", "83810205");

    vector<pair<int, int>> longitudes = {
        {3, 3}, {5, 7}, {1, 9}, {9, 1}, {7, 2}, {13, 11}, {33, 65},
        {127, 129}, {255, 3}, {1, 513}, {999, 1001}
    };
    for (auto [n, m] : longitudes) {
        for (int rep = 0; rep < 3; rep++) {
            probar(cat, numeroAleatorio(n), numeroAleatorio(m));
        }
    }

    reportar(cat);
}

void pruebasAcarreo() {
    Categoria cat{"Acarreo en x1 + x0 (operandos formados solo por nueves)"};

    for (int n : {2, 3, 4, 5, 7, 8, 15, 16, 31, 64, 100, 256, 512}) {
        string nueves = string(n, '9');
        string esperado = string(n - 1, '9') + "8" + string(n - 1, '0') + "1";
        probar(cat, nueves, nueves, esperado);
    }

    reportar(cat);
}

// Caso exigido: al menos un producto grande (varios cientos de digitos)
void pruebasGrandes() {
    Categoria cat{"Productos grandes (300 a 4096 digitos)"};

    for (int n : {300, 512, 777, 1000, 2048, 4096}) {
        probar(cat, numeroAleatorio(n), numeroAleatorio(n));
    }
    probar(cat, numeroAleatorio(1000), numeroAleatorio(17));
    probar(cat, "1" + string(599, '0') + "1", string(600, '9'), string(1200, '9'));

    reportar(cat);
}

void pruebasAleatorias() {
    Categoria cat{"Aleatorias (1000 pares de 1 a 256 digitos)"};

    for (int rep = 0; rep < 1000; rep++) {
        string A = numeroAleatorio(enteroAleatorio(1, 256));
        string B = numeroAleatorio(enteroAleatorio(1, 256));
        if (enteroAleatorio(1, 5) == 1) {
            A = string(enteroAleatorio(1, 20), '0') + A;
        }
        probar(cat, A, B);
    }

    reportar(cat);
}


int main() {
    cout << "=== Pruebas de correctitud: Karatsuba vs. multiplicacion clasica ===\n\n";

    pruebasReferencia();
    pruebasUnDigito();
    pruebasCerosIzquierda();
    pruebasCerosIntercalados();
    pruebasLongitudesImpares();
    pruebasAcarreo();
    pruebasGrandes();
    pruebasAleatorias();

    cout << "\n";
    if (totalFallos == 0) {
        cout << "RESULTADO: todas las pruebas pasaron (" << totalCasos << " casos).\n";
        return 0;
    }

    cout << "RESULTADO: " << totalFallos << " de " << totalCasos << " casos fallaron en "
         << categoriasFallidas << " categoria(s).\n";
    return 1;
}
