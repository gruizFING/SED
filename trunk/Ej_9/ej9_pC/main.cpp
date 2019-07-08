#include <eosim/dist/mt19937.hpp>
#include <eosim/core/experiment.hpp>
#include "hospitalsimple.hpp"
#include "constantes.hpp"

#include <iostream>
#include <fstream>
#include "math.h"
const unsigned int repeticiones = 30;
using namespace std;

int main () {
	string d;
    using namespace eosim::core;
    double x = 0;
    double s = 0;
    double x_techo = 9.93685;
    double s_techo;
    //repito el experimento una cierta cantidad de veces
    for (int i = 0; i < repeticiones; i++) {
        HospitalSimple m(cantCamas, tasaArribos, mediaEstadia, desviacionEstadia);
        Experiment e;
        //std::cout << "Arranco ...\n";
        m.connectToExp(&e);
        e.setSeed((unsigned long) i + 129);
        e.run(763.0);

        m.lCola.reset();
        m.tEspera.reset();
        m.lColaTS.reset();

        e.run(2000.0);
        //std::cout << "Termine ...\n\n\n";
		//m.lCola.print(30);
		//std::cout << '\n';
		//m.tEspera.print(30);
		//std::cout << '\n';
        //m.lColaTS.print(30);
        x += m.lCola.getMean();
        s += pow(m.lCola.getMean() - x_techo,2);
        cout << m.lCola.getMean() << '\n';

    }
    cout << "x_techo:" << x/repeticiones << '\n';
    s_techo = sqrt(s/(repeticiones-1));
    cout << "s:" << s_techo << '\n';
    cout << "Intervalo al 95%"<< '\n';
    cout << x_techo - (2.045)*((s_techo)/sqrt(30)) << " < u < " << x_techo + (2.045)*((s_techo)/sqrt(30)) << '\n';
    cout << "Intervalo al 99%"<< '\n';
    cout << x_techo - (2.756)*((s_techo)/sqrt(30)) << " < u < " << x_techo + (2.756)*((s_techo)/sqrt(30)) << '\n';
    cin >> d;
}

