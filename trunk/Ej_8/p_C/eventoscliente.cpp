#include "eventoscliente.hpp"
#include "banco.hpp"
#include "cliente.hpp"
#include <iostream>

using namespace eosim::core;

// en el constructor se utiliza el identificador definido en eventoscliente.hpp
ClienteFeeder::ClienteFeeder(Model& model): BEvent(clienteF, model) {}

ClienteFeeder::~ClienteFeeder() {}

void ClienteFeeder::eventRoutine(Entity* who) {
    Cliente* c = dynamic_cast<Cliente*>(who);
	// se anuncia la llegada del paciente
	std::string tipo = c->retiraDinero ? "retiro" : "deposito";
	std::cout << "Llega el cliente numero " << c->nro << " de tipo '" << tipo << "'. Tiempo: " << who->getClock() << "\n";
	// se castea owner a un HospitalSimple
	Banco& b = dynamic_cast<Banco&>(owner);
	b.q0.push(who);
	b.schedule(0, who, elegirC);

    // creo proximo cliente y se agenda el arribo del un nuevo cliente
    //Setear 1/3 de probabilidades para ser tipo retiro
    unsigned int number = b.numberGen.nextLong() % 3;
	Cliente* nuevoCliente = new Cliente(number == 0 ? true : false);
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
    unsigned int largoFila = -1;
    for (unsigned int i = 0; i < b.cantCajas; i++)
    {
        Caja* caja = b.cajas[i];
        unsigned int largo = caja->disponible ? 0 : 1;
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
        //b.tServicioO.log(timeService);
        if (caja->soloRetiro)
            b.schedule( b.distTiempoServicioR.sample(), cliente, salidaC);
        else
            b.schedule(b.distTiempoServicioNR.sample(), cliente, salidaC);
    }
    else {
        // pongo al cliente en la fila de la caja elegida
        b.filasCajas[numeroFila]->push(cliente);
        std::cout << "El cliente numero " << cliente->nro << " pasa a esperar en la fila de la caja " << numeroFila + 1 <<  ". Tiempo: " << b.getSimTime() << "\n\n";
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
        // Agarro al primero de la cola sin sacarlo
        Cliente* nuevoClienteCaja = dynamic_cast<Cliente*>(b.filasCajas[cliente->cajaElegida]->pop());
        std::cout << "La caja " << nuevoClienteCaja->cajaElegida + 1 << " pasa a atender el cliente numero " << nuevoClienteCaja->nro << ". Tiempo: " << b.getSimTime() << "\n\n";
        Caja* caja = b.cajas[cliente->cajaElegida];
        //b.tServicioO.log(timeService);
        if (caja->soloRetiro)
            b.schedule(b.distTiempoServicioR.sample(), nuevoClienteCaja, salidaC);
        else
            b.schedule(b.distTiempoServicioNR.sample(), nuevoClienteCaja, salidaC);
    }
    else {
        b.cajas[cliente->cajaElegida]->disponible = true;
        std::cout << "La caja " << cliente->cajaElegida + 1 << " queda disponible. Tiempo: " << b.getSimTime() << "\n\n";
    }
    delete who;
}




