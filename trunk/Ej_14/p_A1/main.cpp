#include <eosim/dist/mt19937.hpp>
#include <eosim/core/experiment.hpp>

#include <iostream>
#include <fstream>
#include "banco.cpp"
#include "constantes.hpp"

const unsigned int repeticiones = 1;

int main () {
    //Recoleccion de datos
    double x_espera = 0;
    double s_espera = 0;
    double xi_espera[repeticiones];

    double x_cantClientes = 0;
    double s_cantClientes = 0;
    double xi_cantClientes[repeticiones];

    double x_largoColas = 0;
    double s_largoColas = 0;
    double xi_largoColas[repeticiones];

    // Variables del modelo
    unsigned int cantCajas = cantCajasM;
    unsigned int minutosSimulacion = minutosSimulacionMin;

    //Para el analisis de sensibilidad, 3 y 4 cajas
    //unsigned int cantCajas = cantCajasMin;
    //unsigned int cantCajas = cantCajasMax;
    //unsigned int minutosSimulacion = minutosSimulacionAS;

    double tiempoEntreArribos = tiempoEntreArribosP;
    bool cajasSoloRetiro[4] = {false, true, false, false}; // Caja 2 solo retiro en este caso
    double tiempoServicioM1 = tiempoServicioNRm, tiempoServicioM2 = tiempoServicioSRm, tiempoServicioS1 = tiempoServicioNRs, tiempoServicioS2 = tiempoServicioSRs;
    std::string s;
    using namespace eosim::core;
    std::cout << "Arranca la simulacion...\n\n";
    //repito el experimento una cierta cantidad de veces
    for (unsigned int i = 1; i <= repeticiones; i++) {
        std::cout << "Repeticion " << i << "\n\n";
        Banco b(cantCajas, cajasSoloRetiro, tiempoEntreArribos,
                tiempoServicioM1, tiempoServicioM2, tiempoServicioS1, tiempoServicioS2);
        Experiment e;
        b.connectToExp(&e);
        e.setSeed((unsigned long) i + 256);
        e.run(60.0*minutosSimulacion);

        std::cout << "Fin Repeticion " << i << "\n\n";

        // Recoleccion de los datos de medias
        xi_espera[i-1] = b.tEsperaO.getMean();
        x_espera += xi_espera[i-1];
        xi_cantClientes[i-1] = b.cantClientesTW.getMean();
        x_cantClientes += xi_cantClientes[i-1];
        xi_largoColas[i-1] = b.lColaCajasTW.getMean();
        x_largoColas += xi_largoColas[i-1];

        // Imprimimos los histogramas para la ultima repeticion
        if (i == repeticiones) {
            b.tEsperaO.print(50, "tEsperaObs.txt");
            b.cantClientesTW.print(50, "cantClientesTW.txt");
            b.lColaCajasTW.print(50, "largoColasCajasTW.txt");
            //b.tServicioO.print(50, "tServicio.txt");

            unsigned int cantCellsTS = (minutosSimulacion * 60) / timeIntervalRDTSseg;
            b.cantClientesTS.table(cantCellsTS, "cantClientesTS.txt");
            b.tEsperaMeanTS.table(cantCellsTS, "tEsperaMeanTS.txt");
            b.lColaCajasMeanTS.table(cantCellsTS, "largoColasCajasMeanTS.txt");
        }
    }

    //Analsis de los resultados
    x_espera /= repeticiones;
    x_cantClientes /= repeticiones;
    x_largoColas /= repeticiones;

    // Calculo los s ahora sabiendo los x promedio
    for (unsigned int i = 0; i < repeticiones; i++) {
        s_espera += pow(xi_espera[i] - x_espera, 2);
        s_cantClientes += pow(xi_cantClientes[i] - x_cantClientes, 2);
        s_largoColas += pow(xi_largoColas[i] - x_largoColas, 2);
    }
    s_espera = repeticiones > 1 ? sqrt(s_espera / (repeticiones - 1)) : sqrt(s_espera); // 1 repeticion ..
    s_cantClientes = repeticiones > 1 ? sqrt(s_cantClientes / (repeticiones - 1)) : sqrt(s_cantClientes);
    s_largoColas = repeticiones > 1 ? sqrt(s_largoColas / (repeticiones - 1)) : sqrt(s_largoColas);

	using namespace std;
    cout << "Tiempo de cada simulacion: " << minutosSimulacion << " minutos. Numero de repeticiones: " << repeticiones << ". " << std::endl << "Cajas abiertas: " << cantCajas << std::endl << std::endl;
    cout << "Media x para los tiempos de espera: " << x_espera << endl;
    cout << "Desviacion s para los tiempos de espera: " << s_espera << endl << endl;
    cout << "Media x para la cantidad de clientes: " << x_cantClientes << endl;
    cout << "Desviacion s para la cantidad de clientes: " << s_cantClientes << endl << endl;
    cout << "Media x para los largos de las colas: " << x_largoColas << endl;
    cout << "Desviacion s para los largos de las colas: " << s_largoColas << endl << endl;

    // Calculo de intervalos de confianza para las medias
    cout << "Intervalo al 95% para los tiempos de espera" << endl;
    cout << x_espera - 2.045 * (s_espera / sqrt(repeticiones)) << " < Mu < " << x_espera + 2.045 * (s_espera / sqrt(repeticiones)) << endl;
    cout << "Intervalo al 99% para los tiempos de espera"<< endl;
    cout << x_espera - 2.756 * (s_espera / sqrt(repeticiones)) << " < Mu < " << x_espera + 2.756 * (s_espera / sqrt(repeticiones)) << endl << endl;

    cout << "Intervalo al 95% para la cantidad de clientes" << endl;
    cout << x_cantClientes - 2.045 * (s_cantClientes / sqrt(repeticiones)) << " < Mu < " << x_cantClientes + 2.045 * (s_cantClientes / sqrt(repeticiones)) << endl;
    cout << "Intervalo al 99% para la cantidad de clientes"<< endl;
    cout << x_cantClientes - 2.756 * (s_cantClientes / sqrt(repeticiones)) << " < Mu < " << x_cantClientes + 2.756 * (s_cantClientes / sqrt(repeticiones)) << endl << endl;

    cout << "Intervalo al 95% para los largos de las colas" << endl;
    cout << x_largoColas - 2.045 * (s_largoColas / sqrt(repeticiones)) << " < Mu < " << x_largoColas + 2.045 * (s_largoColas / sqrt(repeticiones)) << endl;
    cout << "Intervalo al 99% para los largos de las colas"<< endl;
    cout << x_largoColas - 2.756 * (s_largoColas / sqrt(repeticiones)) << " < Mu < " << x_largoColas + 2.756 * (s_largoColas / sqrt(repeticiones)) << endl << endl;

    cout << "Fin de la simulacion ...\n\n\n";
    cin >> s;

    return 0;
}
