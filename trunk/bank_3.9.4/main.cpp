#include <eosim/dist/mt19937.hpp>
#include <eosim/core/experiment.hpp>

#include <iostream>
#include <fstream>
#include "banco.cpp"

const unsigned int repeticiones = 1;

int main () {
    // Variables del modelo
    unsigned int cantCajas = 4;
    double tasaArribos = 60.0; //1 cliente cada 60 sec
    double tiempoServicio = 180.0; //180 sec de atencion
    unsigned int minutosSimulacion = 15;
    bool cajasSoloRetiro[4] = {false, true, false, false}; // Caja 2 solo retiro en este caso

	std::string s;
    using namespace eosim::core;
    //repito el experimento una cierta cantidad de veces
    for (unsigned int i = 0; i < repeticiones; i++) {
        Banco b(cantCajas, cajasSoloRetiro, tasaArribos, tiempoServicio);
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
