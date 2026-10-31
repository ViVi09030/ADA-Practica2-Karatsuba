/*
 * Descripcion:
 * Genera la grafica (SVG) y la tabla de tiempos del analisis
 * experimental a partir del CSV producido por el benchmark.
 */

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const double EXPONENTE_KARATSUBA = log2(3.0);
const int DESDE_N_EXPONENTE = 256;

const string COLOR_CLASICA = "#c0392b";
const string COLOR_KARATSUBA = "#2471a3";
const string COLOR_RAZON = "#7d3c98";
const string COLOR_EJES = "#333333";
const string COLOR_REJILLA = "#e3e3e3";

struct Datos {
    vector<int> n;
    vector<double> clasica;
    vector<double> karatsuba;
    bool todoCorrecto = true;
};

struct Panel {
    double x, y, ancho, alto;
    double xMin, xMax, yMin, yMax;

    double px(double valor) const {
        return x + (log2(valor) - log2(xMin)) / (log2(xMax) - log2(xMin)) * ancho;
    }
    double py(double valor) const {
        return y + alto - (log10(valor) - log10(yMin)) / (log10(yMax) - log10(yMin)) * alto;
    }
};


vector<string> separar(const string& linea) {
    vector<string> campos;
    string campo;
    stringstream ss(linea);
    while (getline(ss, campo, ',')) {
        if (!campo.empty() && campo.back() == '\r') campo.pop_back();
        campos.push_back(campo);
    }
    return campos;
}

bool leerCsv(const string& ruta, Datos& d) {
    ifstream archivo(ruta);
    if (!archivo) {
        return false;
    }

    string linea;
    getline(archivo, linea);
    vector<string> encabezado = separar(linea);

    int colN = -1, colC = -1, colK = -1, colOk = -1;
    for (int i = 0; i < (int)encabezado.size(); i++) {
        if (encabezado[i] == "n") colN = i;
        if (encabezado[i] == "clasica_us") colC = i;
        if (encabezado[i] == "karatsuba_us") colK = i;
        if (encabezado[i] == "correcto") colOk = i;
    }
    if (colN < 0 || colC < 0 || colK < 0) {
        return false;
    }

    while (getline(archivo, linea)) {
        vector<string> c = separar(linea);
        if ((int)c.size() <= max(colN, max(colC, colK))) continue;
        d.n.push_back(stoi(c[colN]));
        d.clasica.push_back(stod(c[colC]));
        d.karatsuba.push_back(stod(c[colK]));
        if (colOk >= 0 && colOk < (int)c.size() && c[colOk] != "si") {
            d.todoCorrecto = false;
        }
    }
    return !d.n.empty();
}

// Pendiente de la recta de minimos cuadrados de log(t) vs log(n): si t ~ c * n^k, estima k
double exponenteEmpirico(const vector<int>& n, const vector<double>& t, int desde) {
    vector<double> xs, ys;
    for (size_t i = 0; i < n.size(); i++) {
        if (n[i] >= desde && t[i] > 0) {
            xs.push_back(log((double)n[i]));
            ys.push_back(log(t[i]));
        }
    }
    if (xs.size() < 2) {
        return NAN;
    }

    double xm = 0, ym = 0;
    for (size_t i = 0; i < xs.size(); i++) {
        xm += xs[i];
        ym += ys[i];
    }
    xm /= xs.size();
    ym /= ys.size();

    double num = 0, den = 0;
    for (size_t i = 0; i < xs.size(); i++) {
        num += (xs[i] - xm) * (ys[i] - ym);
        den += (xs[i] - xm) * (xs[i] - xm);
    }
    return num / den;
}

// Menor n a partir del cual Karatsuba gana en todos los tamanos siguientes
int puntoCruce(const Datos& d) {
    int cruce = -1;
    for (int i = (int)d.n.size() - 1; i >= 0; i--) {
        if (d.karatsuba[i] < d.clasica[i]) {
            cruce = d.n[i];
        } else {
            break;
        }
    }
    return cruce;
}


string num(double v, int decimales = 1) {
    ostringstream os;
    os << fixed << setprecision(decimales) << v;
    return os.str();
}

string potenciaDe10(int exponente) {
    if (exponente == 0) return "1";
    if (exponente == 1) return "10";
    return "10<tspan dy=\"-6\" font-size=\"9\">" + to_string(exponente) + "</tspan>";
}

string linea(double x1, double y1, double x2, double y2, const string& color,
             double grosor, const string& extra = "") {
    return "<line x1=\"" + num(x1) + "\" y1=\"" + num(y1) + "\" x2=\"" + num(x2) +
           "\" y2=\"" + num(y2) + "\" stroke=\"" + color + "\" stroke-width=\"" +
           num(grosor) + "\" " + extra + "/>\n";
}

string texto(double x, double y, const string& contenido, const string& atributos = "") {
    return "<text x=\"" + num(x) + "\" y=\"" + num(y) + "\" " + atributos + ">" +
           contenido + "</text>\n";
}

string polilinea(const Panel& p, const vector<int>& n, const vector<double>& t,
                 const string& color, const string& extra = "") {
    string puntos;
    for (size_t i = 0; i < n.size(); i++) {
        puntos += num(p.px(n[i])) + "," + num(p.py(t[i])) + " ";
    }
    return "<polyline points=\"" + puntos + "\" fill=\"none\" stroke=\"" + color +
           "\" stroke-width=\"2\" " + extra + "/>\n";
}

string marcadores(const Panel& p, const vector<int>& n, const vector<double>& t,
                  const string& color, bool cuadrados) {
    string s;
    for (size_t i = 0; i < n.size(); i++) {
        double x = p.px(n[i]), y = p.py(t[i]);
        if (cuadrados) {
            s += "<rect x=\"" + num(x - 4) + "\" y=\"" + num(y - 4) +
                 "\" width=\"8\" height=\"8\" fill=\"" + color + "\"/>\n";
        } else {
            s += "<circle cx=\"" + num(x) + "\" cy=\"" + num(y) + "\" r=\"4.5\" fill=\"" +
                 color + "\"/>\n";
        }
    }
    return s;
}

string ejes(const Panel& p, const string& titulo, const string& etiquetaY) {
    string s;

    for (int e = (int)floor(log10(p.yMin)); e <= (int)ceil(log10(p.yMax)); e++) {
        double v = pow(10.0, e);
        if (v < p.yMin * 0.999 || v > p.yMax * 1.001) continue;
        double y = p.py(v);
        s += linea(p.x, y, p.x + p.ancho, y, COLOR_REJILLA, 1);
        s += texto(p.x - 8, y + 4, potenciaDe10(e),
                   "text-anchor=\"end\" font-size=\"11\" fill=\"" + COLOR_EJES + "\"");

        bool pocasDecadas = log10(p.yMax / p.yMin) <= 3.01;
        for (int factor : {2, 5}) {
            double w = v * factor;
            if (w >= p.yMax || !pocasDecadas) continue;
            double yw = p.py(w);
            s += linea(p.x, yw, p.x + p.ancho, yw, COLOR_REJILLA, 0.6);
            s += texto(p.x - 8, yw + 4, num(w, e < 0 ? -e : 0),
                       "text-anchor=\"end\" font-size=\"10\" fill=\"#777777\"");
        }
    }

    for (double v = p.xMin; v <= p.xMax * 1.001; v *= 2) {
        double x = p.px(v);
        s += linea(x, p.y, x, p.y + p.alto, COLOR_REJILLA, 1);
        s += texto(x, p.y + p.alto + 17, to_string((long long)v),
                   "text-anchor=\"middle\" font-size=\"10\" fill=\"" + COLOR_EJES + "\"");
    }

    s += "<rect x=\"" + num(p.x) + "\" y=\"" + num(p.y) + "\" width=\"" + num(p.ancho) +
         "\" height=\"" + num(p.alto) + "\" fill=\"none\" stroke=\"" + COLOR_EJES + "\"/>\n";

    s += texto(p.x + p.ancho / 2, p.y - 14, titulo,
               "text-anchor=\"middle\" font-size=\"15\" font-weight=\"bold\" fill=\"" + COLOR_EJES + "\"");
    s += texto(p.x + p.ancho / 2, p.y + p.alto + 40, "N&#250;mero de d&#237;gitos n",
               "text-anchor=\"middle\" font-size=\"12\" fill=\"" + COLOR_EJES + "\"");
    s += texto(0, 0, etiquetaY,
               "text-anchor=\"middle\" font-size=\"12\" fill=\"" + COLOR_EJES +
               "\" transform=\"translate(" + num(p.x - 52) + "," + num(p.y + p.alto / 2) +
               ") rotate(-90)\"");
    return s;
}

// Limites de un eje logaritmico en potencias de 10 que contienen todos los valores
void limitesLog(const vector<double>& valores, double& minimo, double& maximo) {
    double lo = valores[0], hi = valores[0];
    for (double v : valores) {
        if (v < lo) lo = v;
        if (v > hi) hi = v;
    }
    minimo = pow(10.0, floor(log10(lo)));
    maximo = pow(10.0, ceil(log10(hi)));
    if (minimo == maximo) maximo *= 10;
}

bool guardarSvg(const string& ruta, const Datos& d, int cruce) {
    ofstream svg(ruta);
    if (!svg) {
        return false;
    }

    const double ANCHO = 1200, ALTO = 500;
    int nMin = d.n.front(), nMax = d.n.back();

    vector<double> todos = d.clasica;
    todos.insert(todos.end(), d.karatsuba.begin(), d.karatsuba.end());
    vector<double> referencia2, referenciaK;
    double c2 = d.clasica.back() / pow((double)nMax, 2.0);
    double ck = d.karatsuba.back() / pow((double)nMax, EXPONENTE_KARATSUBA);
    for (int n : d.n) {
        referencia2.push_back(c2 * pow((double)n, 2.0));
        referenciaK.push_back(ck * pow((double)n, EXPONENTE_KARATSUBA));
    }
    vector<double> rango = todos;
    rango.insert(rango.end(), referencia2.begin(), referencia2.end());
    rango.insert(rango.end(), referenciaK.begin(), referenciaK.end());

    Panel tiempos{85, 50, 470, 370, (double)nMin, (double)nMax, 0, 0};
    limitesLog(rango, tiempos.yMin, tiempos.yMax);

    vector<double> razon;
    for (size_t i = 0; i < d.n.size(); i++) {
        razon.push_back(d.karatsuba[i] / d.clasica[i]);
    }
    vector<double> rangoRazon = razon;
    rangoRazon.push_back(1.0);
    Panel razones{705, 50, 470, 370, (double)nMin, (double)nMax, 0, 0};
    limitesLog(rangoRazon, razones.yMin, razones.yMax);

    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << ANCHO << "\" height=\"" << ALTO
        << "\" viewBox=\"0 0 " << ANCHO << " " << ALTO
        << "\" font-family=\"Arial, Helvetica, sans-serif\">\n";
    svg << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";

    svg << ejes(tiempos, "Tiempo de ejecuci&#243;n (escala log-log)",
                "Tiempo por multiplicaci&#243;n (&#181;s)");
    svg << polilinea(tiempos, d.n, referencia2, COLOR_CLASICA,
                     "stroke-dasharray=\"7,5\" opacity=\"0.45\"");
    svg << polilinea(tiempos, d.n, referenciaK, COLOR_KARATSUBA,
                     "stroke-dasharray=\"2,4\" opacity=\"0.7\"");
    svg << polilinea(tiempos, d.n, d.clasica, COLOR_CLASICA);
    svg << polilinea(tiempos, d.n, d.karatsuba, COLOR_KARATSUBA);
    svg << marcadores(tiempos, d.n, d.clasica, COLOR_CLASICA, false);
    svg << marcadores(tiempos, d.n, d.karatsuba, COLOR_KARATSUBA, true);

    if (cruce != -1) {
        double x = tiempos.px(cruce);
        svg << linea(x, tiempos.y, x, tiempos.y + tiempos.alto, "gray", 1.2,
                     "stroke-dasharray=\"8,3,2,3\"");
        svg << texto(x + 6, tiempos.y + tiempos.alto - 10, "cruce: n = " + to_string(cruce),
                     "font-size=\"11\" fill=\"gray\"");
    }

    double lx = tiempos.x + 12, ly = tiempos.y + 12;
    svg << "<rect x=\"" << num(lx) << "\" y=\"" << num(ly)
        << "\" width=\"205\" height=\"86\" fill=\"white\" stroke=\"#bbbbbb\"/>\n";
    svg << linea(lx + 10, ly + 17, lx + 36, ly + 17, COLOR_CLASICA, 2);
    svg << "<circle cx=\"" << num(lx + 23) << "\" cy=\"" << num(ly + 17) << "\" r=\"4.5\" fill=\""
        << COLOR_CLASICA << "\"/>\n";
    svg << texto(lx + 44, ly + 21, "Cl&#225;sica", "font-size=\"12\"");
    svg << linea(lx + 10, ly + 36, lx + 36, ly + 36, COLOR_KARATSUBA, 2);
    svg << "<rect x=\"" << num(lx + 19) << "\" y=\"" << num(ly + 32)
        << "\" width=\"8\" height=\"8\" fill=\"" << COLOR_KARATSUBA << "\"/>\n";
    svg << texto(lx + 44, ly + 40, "Karatsuba", "font-size=\"12\"");
    svg << linea(lx + 10, ly + 55, lx + 36, ly + 55, COLOR_CLASICA, 2,
                 "stroke-dasharray=\"7,5\" opacity=\"0.45\"");
    svg << texto(lx + 44, ly + 59, "Referencia &#920;(n&#178;)", "font-size=\"12\"");
    svg << linea(lx + 10, ly + 74, lx + 36, ly + 74, COLOR_KARATSUBA, 2,
                 "stroke-dasharray=\"2,4\" opacity=\"0.7\"");
    svg << texto(lx + 44, ly + 78,
                 "Referencia &#920;(n<tspan dy=\"-5\" font-size=\"9\">log&#8322;3</tspan><tspan dy=\"5\">)</tspan>",
                 "font-size=\"12\"");

    svg << ejes(razones, "Raz&#243;n de tiempos (&lt; 1: Karatsuba gana)",
                "Tiempo Karatsuba / tiempo cl&#225;sica");
    double y1 = razones.py(1.0);
    svg << linea(razones.x, y1, razones.x + razones.ancho, y1, "gray", 1.2,
                 "stroke-dasharray=\"6,4\"");
    svg << polilinea(razones, d.n, razon, COLOR_RAZON);
    svg << marcadores(razones, d.n, razon, COLOR_RAZON, false);

    svg << "</svg>\n";
    return true;
}

bool guardarTabla(const string& ruta, const Datos& d) {
    ofstream md(ruta);
    if (!md) {
        return false;
    }
    md << "| n | Clásica (µs) | Karatsuba (µs) | Karatsuba / Clásica |\n";
    md << "|---:|---:|---:|---:|\n";
    for (size_t i = 0; i < d.n.size(); i++) {
        md << "| " << d.n[i] << " | " << num(d.clasica[i], 3) << " | " << num(d.karatsuba[i], 3)
           << " | " << num(d.karatsuba[i] / d.clasica[i], 2) << " |\n";
    }
    return true;
}


int main(int argc, char* argv[]) {
#ifdef RAIZ_PROYECTO
    string carpeta = string(RAIZ_PROYECTO) + "/resultados/";
#else
    string carpeta = "resultados/";
#endif
    string rutaCsv = (argc > 1) ? argv[1] : carpeta + "tiempos.csv";
    if (argc > 1) {
        size_t barra = rutaCsv.find_last_of("/\\");
        carpeta = (barra == string::npos) ? "" : rutaCsv.substr(0, barra + 1);
    }

    Datos d;
    if (!leerCsv(rutaCsv, d)) {
        cout << "No se pudo leer " << rutaCsv << ". Ejecute primero el benchmark.\n";
        return 1;
    }
    if (!d.todoCorrecto) {
        cout << "AVISO: hay tamanos en los que Karatsuba no coincidio con la clasica.\n";
    }

    int cruce = puntoCruce(d);
    double expC = exponenteEmpirico(d.n, d.clasica, DESDE_N_EXPONENTE);
    double expK = exponenteEmpirico(d.n, d.karatsuba, DESDE_N_EXPONENTE);

    string rutaSvg = carpeta + "grafica_tiempos.svg";
    string rutaMd = carpeta + "tabla_tiempos.md";
    bool okSvg = guardarSvg(rutaSvg, d, cruce);
    bool okMd = guardarTabla(rutaMd, d);

    cout << setw(6) << "n" << setw(16) << "Clasica (us)" << setw(16) << "Karatsuba (us)"
         << setw(10) << "K / C" << "\n";
    for (size_t i = 0; i < d.n.size(); i++) {
        cout << setw(6) << d.n[i] << setw(16) << num(d.clasica[i], 3) << setw(16)
             << num(d.karatsuba[i], 3) << setw(10) << num(d.karatsuba[i] / d.clasica[i], 2) << "\n";
    }
    cout << "\n";

    if (cruce == -1) {
        cout << "Punto de cruce: no aparece en el rango medido.\n";
    } else {
        cout << "Punto de cruce: n = " << cruce << " digitos.\n";
    }
    cout << "Exponente empirico (n >= " << DESDE_N_EXPONENTE << "): clasica ~ " << num(expC, 3)
         << " (teorico 2), karatsuba ~ " << num(expK, 3) << " (teorico "
         << num(EXPONENTE_KARATSUBA, 3) << ")\n";
    cout << (okSvg ? "Grafica guardada en: " : "No se pudo escribir: ") << rutaSvg << "\n";
    cout << (okMd ? "Tabla guardada en:   " : "No se pudo escribir: ") << rutaMd << "\n";
    return 0;
}
