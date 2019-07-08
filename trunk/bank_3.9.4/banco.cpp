#include "banco.hpp"
#include <eosim/core/entity.hpp>
#include <eosim/dist/numbergenerator.hpp>
#include <iostream>

using namespace eosim::core;
using namespace eosim::dist;

using namespace std;

Banco::Banco(unsigned int cantC, bool cajasSoloRetiro[], double tArribos, double tServicio):
// se contruyen los eventos B, los eventos C, las distribuciones, los recursos y los histogramas
								tasaArribos(tArribos),
								tiempoServicioCliente(tServicio),
								cantCajas(cantC),
								cF(*this),
								cS(*this),
								eC(*this),
								tEspera("Tiempos de Espera"),
								lCola("Largos Medios de Colas", *this)
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
}

void Banco::doInitialSchedules() {
	// agendo el primer paciente, probabilidad de 1/3 de ser operacion de retiro
	nroCliente = 1;
	unsigned int number = numberGen.nextLong() % 3;
	Cliente* cliente = new Cliente(number == 0 ? true : false);
	cliente->nro = nroCliente;
	schedule(0.0, cliente, clienteF);
}
