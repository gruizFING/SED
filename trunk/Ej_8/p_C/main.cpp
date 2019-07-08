#include <eosim/dist/mt19937.hpp>
#include <eosim/core/experiment.hpp>

#include <iostream>
#include <fstream>
#include "banco.cpp"
#include "constantes.hpp"

const unsigned int repeticiones = 1;

int main () {
    // Variables del modelo
    unsigned int cantCajas = cantCajasM;
    unsigned int minutosSimulacion = minutosSimulacionMin;
    double tiempoEntreArribos = tiempoEntreArribosP;
    double tiempoServicioM1 = tiempoServicioNRm, tiempoServicioM2 = tiempoServicioSRm, tiempoServicioS1 = tiempoServicioNRs, tiempoServicioS2 = tiempoServicioSRs;
    bool cajasSoloRetiro[4] = {false, true, false, false}; // Caja 2 solo retiro en este caso

	std::string s;
    using namespace eosim::core;
    //repito el experimento una cierta cantidad de veces
    for (unsigned int i = 0; i < repeticiones; i++) {
        Banco b(cantCajas, cajasSoloRetiro, tiempoEntreArribos,
                tiempoServicioM1, tiempoServicioM2, tiempoServicioS1, tiempoServicioS2);
        Experiment e;
        std::cout << "Arranca la simulacion ...\n\n";
        b.connectToExp(&e);
        e.setSeed((unsigned long) i + 129);
        e.run(60.0*minutosSimulacion);
        std::cout << "Termina la simulacion ...\n\n";
        std::cin >> s;
    }
    return 0;
}
