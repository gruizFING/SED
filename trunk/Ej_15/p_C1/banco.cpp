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
								queSw(*this),
								rdTS(*this),
								cantCajas(cantC),
								distArribosClientes(MT19937, tiempoEntreArribos),
								distTiempoServicioNR(MT19937, tiempoServicioM1, tiempoServicioS1),
								distTiempoServicioR(MT19937, tiempoServicioM2, tiempoServicioS2),
								distProbRetiro(MT19937, 0 , 1),
								distProbThinning(MT19937, 0 , 1),
								cantClientesTS("Cantidad de clientes TS", *this),
								tEsperaMeanTS("Media de tiempos de Espera TS", *this),
								lColaCajasMeanTS("Media de largo de colas TS", *this),
								tEsperaO("Tiempos de Espera"),
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
    registerBEvent(&queSw);
    registerBEvent(&rdTS);

    // registro las distribuciones
    registerDist(&distArribosClientes);
    registerDist(&distTiempoServicioNR);
    registerDist(&distTiempoServicioR);
    registerDist(&distProbRetiro);
    registerDist(&distProbThinning);
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

	// agendo el primer registro de datos para los swap de filas
	schedule(timeIntervalSwap, new Entity(), queueC);
}

void Banco::cambiarFila(Cliente* cliente, unsigned int cajaElegida)
{
    std::cout << "El cliente numero " << cliente->nro << " se cambia a la fila de la caja " << cajaElegida + 1 << ". Tiempo: " << this->getSimTime() << "\n\n";
    Caja* caja = this->cajas[cajaElegida];
    // Registro datos del largo de la cola
    //this->lColaCajasTW.log(this->filasCajas[cliente->cajaElegida]->size());
    cliente->cajaElegida = cajaElegida;
    if (caja->disponible) {
        caja->disponible = false;
        std::cout << "La caja " << cajaElegida + 1 << " atiende al cliente numero " << cliente->nro << ". Tiempo: " << this->getSimTime() << "\n\n";

        //Registro tiempo de espera en la cola que tuvo el cliente
        double tiempoEspera = this->getSimTime() - cliente->getClock();
        this->tEsperaO.log(tiempoEspera);

        double stay = this->distTiempoServicioNR.sample();
        this->schedule(stay, cliente, salidaC);

        //Registro largo de cola 1 en el caso que solo 1 siendo atendido..
        this->lColaCajasTW.log(1);
    }
    else {
        // pongo al cliente en la fila de la caja elegida
        this->filasCajas[cajaElegida]->push(cliente);
        std::cout << "El cliente numero " << cliente->nro << " pasa a esperar en la fila de la caja "<< cajaElegida + 1 << ". Tiempo: " << this->getSimTime() << "\n\n";
        //Registro datos de largo de cola
        unsigned int largoCola = this->filasCajas[cajaElegida]->size();
        this->lColaCajasTW.log(largoCola);
    }
}

