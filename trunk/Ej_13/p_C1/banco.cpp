#include "banco.hpp"
#include <eosim/core/entity.hpp>
#include <eosim/dist/numbergenerator.hpp>
#include <iostream>

using namespace eosim::core;
using namespace eosim::dist;

using namespace std;

Banco::Banco(unsigned int cantC, bool cajasSoloRetiro[], double tiempoEntreArribos, double tiempoServicioM1,
                double tiempoServicioM2, double tiempoServicioS1, double tiempoServicioS2):
// se contruyen los eventos B, los eventos C, las distribuciones, los recursos y los histogramas
								cF(*this),
								cS(*this),
								eC(*this),
								rdTS(*this),
								cantCajas(cantC),
								distArribosClientes(MT19937, tiempoEntreArribos),
								distTiempoServicioNR(MT19937, tiempoServicioM1, tiempoServicioS1),
								distTiempoServicioR(MT19937, tiempoServicioM2, tiempoServicioS2),
								distProbRetiro(MT19937, 0 , 1),
								cantClientesTS("Cantidad de clientes TS", *this),
								tEsperaMeanTS("Media de tiempos de Espera TS", *this),
								lColaCajasMeanTS("Media de largo de colas TS", *this),
								tEsperaO("Tiempos de Espera"),
								//tServicioO("Tiempos de Servicio"),
								cantClientesTW("Cantidad de clientes TW", *this),
								lColaCajasTW("Media de largo de colas TW", *this)
    {
        for (unsigned int i = 0; i < cantC; i++) {
            filasCajas.push_back(new eosim::utils::EntityQueueFifo());
            cajas.push_back(new Caja(cajasSoloRetiro[i]));
        }
    }

Banco::~Banco() {}

void Banco::init() {
	// registro los eventos B
	registerBEvent(&cF);
	registerBEvent(&cS);
    registerBEvent(&eC);
    registerBEvent(&rdTS);

    // registro las distribuciones
    registerDist(&distArribosClientes);
    registerDist(&distTiempoServicioNR);
    registerDist(&distTiempoServicioR);
    registerDist(&distProbRetiro);
}

void Banco::doInitialSchedules() {
	// agendo el primer cliente, probabilidad de 1/3 de ser operacion de retiro
	nroCliente = 1;
	cantClientesActuales = 0;
	double probRetiro = this->distProbRetiro.sample();
	Cliente* cliente = new Cliente(probRetiro > ((double)1/3) ? false : true);
	cliente->nro = nroCliente;
	schedule(0.0, cliente, clienteF);

	// agendo el primer registro de datos para los TS
	schedule(timeIntervalRDTSseg, new Entity(), RDTS);
}

