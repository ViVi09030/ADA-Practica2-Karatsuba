#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<double> ns, tc, tk;

// Pendiente de la recta de minimos cuadrados de log(t) contra log(n) para n >= 256:
// si t es proporcional a n^k, la pendiente estima k
double exponente(const vector<double>& t) {
    double sx = 0, sy = 0, sxx = 0, sxy = 0, m = 0;
    for (size_t i = 0; i < ns.size(); i++) {
        if (ns[i] < 256) continue;
        double x = log(ns[i]), y = log(t[i]);
        sx += x; sy += y; sxx += x * x; sxy += x * y; m++;
    }
    return (m * sxy - sx * sy) / (m * sxx - sx * sx);
}

// Coordenadas del area de dibujo: x en log2(n), y en log10(t)
const double X0 = 70, Y0 = 20, ANCHO = 600, ALTO = 380;
double yMin, yMax;
double px(double n) { return X0 + (log2(n) - log2(ns.front())) / (log2(ns.back()) - log2(ns.front())) * ANCHO; }
double py(double t) { return Y0 + ALTO - (log10(t) - yMin) / (yMax - yMin) * ALTO; }

string serie(const vector<double>& t, const string& color) {
    ostringstream s;
    s << "<polyline fill='none' stroke='" << color << "' stroke-width='2' points='";
    for (size_t i = 0; i < ns.size(); i++) s << px(ns[i]) << "," << py(t[i]) << " ";
    s << "'/>\n";
    for (size_t i = 0; i < ns.size(); i++)
        s << "<circle cx='" << px(ns[i]) << "' cy='" << py(t[i]) << "' r='3.5' fill='" << color << "'/>\n";
    return s.str();
}

int main() {
#ifdef RAIZ_PROYECTO
    string carpeta = string(RAIZ_PROYECTO) + "/resultados";
#else
    string carpeta = "resultados";
#endif
    ifstream csv(carpeta + "/tiempos.csv");
    string linea;
    getline(csv, linea);
    while (getline(csv, linea)) {
        double n, c, k;
        char coma;
        if (stringstream(linea) >> n >> coma >> c >> coma >> k) {
            ns.push_back(n); tc.push_back(c); tk.push_back(k);
        }
    }
    if (ns.size() < 2) {
        cout << "No se pudo leer " << carpeta << "/tiempos.csv\n";
        return 1;
    }

    int cruce = -1;
    for (size_t i = 0; i < ns.size(); i++) {
        if (tk[i] >= tc[i]) cruce = -1;
        else if (cruce == -1) cruce = (int)ns[i];
    }

    yMin = 1e18; yMax = -1e18;
    for (size_t i = 0; i < ns.size(); i++) {
        yMin = min(yMin, floor(log10(min(tc[i], tk[i]))));
        yMax = max(yMax, ceil(log10(max(tc[i], tk[i]))));
    }

    ofstream svg(carpeta + "/grafica_tiempos.svg");
    svg << "<svg xmlns='http://www.w3.org/2000/svg' width='700' height='460' font-family='Arial' font-size='12'>\n"
        << "<rect width='100%' height='100%' fill='white'/>\n";
    for (double e = yMin; e <= yMax; e++) {
        svg << "<line x1='" << X0 << "' x2='" << X0 + ANCHO << "' y1='" << py(pow(10, e)) << "' y2='"
            << py(pow(10, e)) << "' stroke='#ddd'/>\n"
            << "<text x='" << X0 - 6 << "' y='" << py(pow(10, e)) + 4 << "' text-anchor='end'>1e"
            << e << "</text>\n";
    }
    for (double n : ns) {
        svg << "<line x1='" << px(n) << "' x2='" << px(n) << "' y1='" << Y0 << "' y2='" << Y0 + ALTO
            << "' stroke='#ddd'/>\n"
            << "<text x='" << px(n) << "' y='" << Y0 + ALTO + 16 << "' text-anchor='middle'>" << n << "</text>\n";
    }
    svg << "<rect x='" << X0 << "' y='" << Y0 << "' width='" << ANCHO << "' height='" << ALTO
        << "' fill='none' stroke='black'/>\n"
        << "<text x='" << X0 + ANCHO / 2 << "' y='450' text-anchor='middle'>Numero de digitos n</text>\n"
        << "<text transform='translate(18," << Y0 + ALTO / 2 << ") rotate(-90)' text-anchor='middle'>"
        << "Tiempo por multiplicacion (microsegundos)</text>\n";
    if (cruce != -1)
        svg << "<line x1='" << px(cruce) << "' x2='" << px(cruce) << "' y1='" << Y0 << "' y2='" << Y0 + ALTO
            << "' stroke='gray' stroke-dasharray='5,4'/>\n";
    svg << serie(tc, "#c0392b") << serie(tk, "#2471a3")
        << "<text x='" << X0 + 12 << "' y='" << Y0 + 20 << "' fill='#c0392b'>Clasica</text>\n"
        << "<text x='" << X0 + 12 << "' y='" << Y0 + 38 << "' fill='#2471a3'>Karatsuba</text>\n"
        << "</svg>\n";

    if (cruce == -1) cout << "Punto de cruce: no aparece en el rango medido\n";
    else cout << "Punto de cruce: n = " << cruce << "\n";
    cout << "Exponente empirico (n >= 256): clasica " << exponente(tc) << ", karatsuba " << exponente(tk) << "\n";
    cout << "Grafica: " << carpeta << "/grafica_tiempos.svg\n";
    return 0;
}
