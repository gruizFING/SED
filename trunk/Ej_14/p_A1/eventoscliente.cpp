#include "eventoscliente.hpp"
#include "banco.hpp"
#include "cliente.hpp"
#include <iostream>
#include <math.h>
#include <eosim/graphic/Display.hpp>

using namespace eosim::core;
using namespace eosim::graphic;

// en el constructor se utiliza el identificador definido en eventoscliente.hpp
ClienteFeeder::ClienteFeeder(Model& model): BEvent(clienteF, model) {}

ClienteFeeder::~ClienteFeeder() {}

void ClienteFeeder::eventRoutine(Entity* who) {
    Cliente* c = dynamic_cast<Cliente*>(who);
    c->spr.setVisible(1);

	// se anuncia la llegada del cliente
	std::string tipo = c->retiraDinero ? "Retiro" : "Otro";
	std::cout << "Llega el cliente numero " << c->nro << " de tipo '" << tipo << "'. Tiempo: " << who->getClock() << "\n";
	// se castea owner a un Banco
	Banco& b = dynamic_cast<Banco&>(owner);
	b.q0.push(who);
	b.schedule(0, who, elegirC);

    // Regristro datos sobre la cantidad de clientes
	b.cantClientesActuales++;
	if (b.cantClientesActuales > b.cantClientesMax)
        b.cantClientesMax = b.cantClientesActuales;
	b.cantClientesTW.log(b.cantClientesActuales);
    b.dispActualizarClientesEnSistema();

    // creo proximo cliente y se agenda el arribo del nuevo cliente
    // 1 de cada 3 son retiro
    double probRetiro = b.distProbRetiro.sample();
	Cliente* nuevoCliente = new Cliente(b.disp, b.puertaEntrada, probRetiro > ((double)1/3) ? false : true);
	nuevoCliente->spr.setVisible(0);
	nuevoCliente->nro = ++b.nroCliente;
	b.schedule(b.distArribosClientes.sample(), nuevoCliente, clienteF);
}

// en el constructor se utiliza el identificador definido en eventoscliente.hpp
elegirCajaCliente::elegirCajaCliente(Model& model): BEvent(elegirC, model) {}

elegirCajaCliente::~elegirCajaCliente() {}

void elegirCajaCliente::eventRoutine(Entity* who) {
    Banco& b = dynamic_cast<Banco&>(owner);
    b.q0.pop();

    // Selecciono la fila del cliente
    Cliente* cliente = dynamic_cast<Cliente*>(who);
    unsigned int numeroFila = 0;
    int largoFila = -1;
    for (unsigned int i = 0; i < b.cantCajas; i++)
    {
        Caja* caja = b.cajas[i];
        int largo = caja->disponible ? 0 : 1;
        largo += b.filasCajas[i]->size();
        if (caja->soloRetiro && cliente->retiraDinero || !caja->soloRetiro && !cliente->retiraDinero) {
            if (largoFila == -1 || largoFila > largo) {
                largoFila = largo;
                numeroFila = i;
            }
        }
    }

    std::cout << "El cliente numero " << cliente->nro << " elige la fila de la caja " << numeroFila + 1 << ". Tiempo: " << b.getSimTime() << "\n";
    cliente->cajaElegida = numeroFila;
    Caja* caja = b.cajas[numeroFila];
    if (caja->disponible) {
        caja->disponible = false;
        std::cout << "La caja " << numeroFila + 1 << " atiende al cliente numero " << cliente->nro << ". Tiempo: " << b.getSimTime() << "\n\n";
        double stay = 0.0;
        if (caja->soloRetiro)
            stay = b.distTiempoServicioR.sample();
        else
            stay = b.distTiempoServicioNR.sample();
        b.schedule(stay, cliente, salidaC);

        //Registro largo de cola 1 en el caso que solo 1 siendo atendido..
        b.lColaCajasTW.log(1);
        if (b.largoFilaMax < 1) {
            b.largoFilaMax = 1;
            b.dispActualizarLargoMaxColas();
        }
        //Registro tiempo de espera para este cliente 0..
        b.tEsperaO.log(0);

        cliente->spr.setMoves(move(b.filasEspera[numeroFila].first, b.filasEspera[numeroFila].second, stay*0.05).move(caja->pos.first - 64, caja->pos.second, stay*0.15));
    }
    else {
        // pongo al cliente en la fila de la caja elegida
        b.filasCajas[numeroFila]->push(cliente);
        std::cout << "El cliente numero " << cliente->nro << " pasa a esperar en la fila de la caja " << numeroFila + 1 <<  ". Tiempo: " << b.getSimTime() << "\n\n";
        //Registro datos de largo de cola
        unsigned int largoCola = b.filasCajas[numeroFila]->size();
        b.lColaCajasTW.log(largoCola);
        if (largoCola > b.largoFilaMax) {
            b.largoFilaMax = largoCola;
            b.dispActualizarLargoMaxColas();
        }

        cliente->spr.setMoves(move(b.filasEspera[numeroFila].first - (largoCola - 1) * 48, b.filasEspera[numeroFila].second, 0));
    }
}

// en el constructor se utiliza el identificador definido en eventoscliente.hpp
SalidaCliente::SalidaCliente(Model& model): BEvent(salidaC, model) {}

SalidaCliente::~SalidaCliente() {}

void SalidaCliente::eventRoutine(Entity* who) {
    Banco& b = dynamic_cast<Banco&>(owner);
    Cliente* cliente = dynamic_cast<Cliente*>(who);

    std::cout << "El cliente numero " << cliente->nro << " de la caja " << cliente->cajaElegida + 1 << " sale del banco. Tiempo: " << b.getSimTime() << "\n";
    if (!b.filasCajas[cliente->cajaElegida]->empty()) {
        // Agarro al primero de la cola
        Cliente* nuevoClienteCaja = dynamic_cast<Cliente*>(b.filasCajas[cliente->cajaElegida]->pop());
        std::cout << "La caja " << nuevoClienteCaja->cajaElegida + 1 << " pasa a atender el cliente numero " << nuevoClienteCaja->nro << ". Tiempo: " << b.getSimTime() << "\n\n";

        //Registro tiempo de espera en la cola que tuvo el cliente
        double tiempoEspera = b.getSimTime() - nuevoClienteCaja->getClock();
        b.tEsperaO.log(tiempoEspera);
        unsigned int tiempoEsperaInt = (unsigned int)(tiempoEspera + 0.5);
        if (tiempoEsperaInt > b.tiempoEsperaMax) {
            b.tiempoEsperaMax = tiempoEsperaInt;
            b.dispActualizarEsperaMax();
        }

        Caja* caja = b.cajas[cliente->cajaElegida];
        double stay = 0.0;
        if (caja->soloRetiro)
            stay = b.distTiempoServicioR.sample();
        else
            stay = b.distTiempoServicioNR.sample();
        b.schedule(stay, nuevoClienteCaja, salidaC);

        nuevoClienteCaja->spr.setMoves(move(caja->pos.first - 64, caja->pos.second, stay*0.05));
        // mover a todos para adelante
		for (unsigned int i = 0; i < b.filasCajas[cliente->cajaElegida]->size(); i++) {
            Cliente* tempClient = dynamic_cast<Cliente*>(b.filasCajas[cliente->cajaElegida]->operator[](i));
			tempClient->spr.setMoves(move(b.filasEspera[cliente->cajaElegida].first - i * 48, b.filasEspera[cliente->cajaElegida].second, 0));
		}
    }
    else {
        b.cajas[cliente->cajaElegida]->disponible = true;
        std::cout << "La caja " << cliente->cajaElegida + 1 << " queda disponible. Tiempo: " << b.getSimTime() << "\n\n";
    }

    // Registro datos del largo de la cola
    b.lColaCajasTW.log(b.filasCajas[cliente->cajaElegida]->size());

    // Registro datos sobre la cantidad de clientes
    b.cantClientesActuales--;
	b.cantClientesTW.log(b.cantClientesActuales);
	b.dispActualizarClientesEnSistema();

    cliente->spr.setMoves(move(b.puertaSalida.first, b.puertaSalida.second, 10).move(b.salida.first, b.salida.second, 5));
    delete who;
}




