#include <eosim/dist/mt19937.hpp>
#include <eosim/core/experiment.hpp>
#include "hospitalsimple.hpp"
#include "constantes.hpp"
#include "statistics.h"

#include <iostream>
#include <fstream>

const int N = 50;

using namespace eosim::core;

int main () {
	std::string s;
    double promediosLargoCola[N];
    double promediosEsperaOperacion[N];

    for (int i = 0; i < N; i++) {
        HospitalSimple m(cantCamasC);
        Experiment e;
        std::cout << "Inicia simulacion \t[" << i+1 <<"]\n";
        m.connectToExp(&e);
        e.setSeed((unsigned long) i + 129);
        e.run(20000.0);
        std::cout << "Finaliza simulacion \t["<< i+1 <<"]\n\n\n";
		//m.lColaCama.print(40);
		std::cout << '\n';
		//m.esperaOperacion.print(40);
        promediosLargoCola[i] = m.lColaCama.getMean();
        promediosEsperaOperacion[i] = m.esperaOperacion.getMean();
    }

    /** Intervalo de Confianza:
    */
    //Largo cola
    double media_LCola = media(promediosLargoCola,N);
    double desv_std_LCola = desv_std(varianza(promediosLargoCola,media_LCola,N));
    //Espera cola
    double media_EOperacion = media(promediosEsperaOperacion,N);
    double desv_std_EOperacion = desv_std(varianza(promediosEsperaOperacion,media_EOperacion,N));

    std::cout << std::endl << "Intervalos de confianza:" << std::endl
              << std::endl
              << "Promedio de largo de las colas para obtener una cama:   [ " << inf_int_confianza(media_LCola,desv_std_LCola,N) <<" , "<< sup_int_confianza(media_LCola,desv_std_LCola,N) << " ]" << std::endl
              << "Promedio de espera para operarse:  [ " << inf_int_confianza(media_EOperacion,desv_std_EOperacion,N) <<" , "<< sup_int_confianza(media_EOperacion,desv_std_EOperacion,N) << " ]" << std::endl;

    std::cin >> s;
}
