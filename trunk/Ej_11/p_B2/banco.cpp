#include "banco.hpp"
#include <eosim/core/entity.hpp>
#include <eosim/dist/numbergenerator.hpp>
#include <iostream>

using namespace eosim::core;
using namespace eosim::dist;

using namespace std;

Banco::Banco(unsigned int cantC, bool cajasSoloRetiro[], double tiempoEntreArribos,
             double tiempoServicioM, double tiempoServicioS):
// se contruyen los eventos B, los eventos C, las distribuciones, los recursos y los histogramas
								cF(*this),
								cS(*this),
								rdTS(*this),
								cantCajas(cantC),
								distArribosClientes(MT19937, tiempoEntreArribos),
								distTiempoServicio(MT19937, tiempoServicioM, tiempoServicioS),
								cantClientesTS("Cantidad de clientes TS", *this),
								tEsperaMeanTS("Media de tiempos de Espera TS", *this),
								lColaCajasMeanTS("Media de largo de la cola TS", *this),
								tEsperaO("Tiempos de Espera"),
								cantClientesTW("Cantidad de clientes TW", *this),
								lColaCajasTW("Media de largo de la cola TW", *this)
    {
        for (unsigned int i = 0; i < cantC; i++) {
            cajas.push_back(new Caja(false)); // Todas las cajas no son de solo retiro
        }
    }

Banco::~Banco() {}

void Banco::init() {
	// registro los eventos B
	registerBEvent(&cF);
	registerBEvent(&cS);
    registerBEvent(&rdTS);

    // registro las distribuciones
    registerDist(&distArribosClientes);
    registerDist(&distTiempoServicio);
}

void Banco::doInitialSchedules() {
	// agendo el primer cliente
	nroCliente = 1;
	cantClientesActuales = 0;
	Cliente* cliente = new Cliente(false); //No se toma el tipo de cliente en este modelo
	cliente->nro = nroCliente;
	schedule(0.0, cliente, clienteF);
	// agendo el primer registro de datos para los TS
	schedule(timeIntervalRDTSseg, new Entity(), RDTS);
}

