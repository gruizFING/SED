#include "eventoscliente.hpp"
#include "banco.hpp"
#include "cliente.hpp"
#include <iostream>
#include <math.h>

using namespace eosim::core;

// en el constructor se utiliza el identificador definido en eventoscliente.hpp
ClienteFeeder::ClienteFeeder(Model& model): BEvent(clienteF, model) {}

ClienteFeeder::~ClienteFeeder() {}

void ClienteFeeder::eventRoutine(Entity* who) {
    Cliente* c = dynamic_cast<Cliente*>(who);
	// se anuncia la llegada del cliente
	std::cout << "Llega el cliente numero " << c->nro << ". Tiempo: " << who->getClock() << "\n";
	// se castea owner a un Banco
	Banco& b = dynamic_cast<Banco&>(owner);

	//Elegir caja disponible o esperar en cola q0
    Cliente* cliente = dynamic_cast<Cliente*>(who);
    int numeroCaja = -1;
    for (unsigned int i = 0; i < b.cantCajas; i++)
    {
        Caja* caja = b.cajas[i];
        if (caja->disponible) { //Agarro la primer caja disponible
            numeroCaja = i;
            break;
        }
    }

    if (numeroCaja == -1) //No hay caja disponible, poner el cliente a esperar en qInicial
    {
        b.qInicial.push(who);
        std::cout << "El cliente numero " << cliente->nro << " pasa a esperar en la cola. Tiempo: " << b.getSimTime() << "\n\n";
        //Registro largo de la cola
        b.lColaCajasTW.log(b.qInicial.size());
    }
    else // Poner al cliente en la caja elegida para ser atendido
    {
        cliente->cajaElegida = numeroCaja;
        Caja* caja = b.cajas[numeroCaja];
        caja->disponible = false;
        std::cout << "La caja " << numeroCaja + 1 << " atiende al cliente numero " << cliente->nro << ". Tiempo: " << b.getSimTime() << "\n\n";
        //Se toma el tiempo de ir a la caja en 10 seg agregandolo al tiempo de servicio
        b.schedule(b.distTiempoServicio.sample() + 10, cliente, salidaC);

        //Registro tiempo de espera para este cliente igual a 0
        b.tEsperaO.log(0);
    }

    // Regristro datos sobre la cantidad de clientes
	b.cantClientesActuales++;
	b.cantClientesTW.log(b.cantClientesActuales);

    // creo proximo cliente y se agenda el arribo del nuevo cliente
	Cliente* nuevoCliente = new Cliente(false);
	nuevoCliente->nro = ++b.nroCliente;
	b.schedule(b.distArribosClientes.sample(), nuevoCliente, clienteF);
}

// en el constructor se utiliza el identificador definido en eventoscliente.hpp
SalidaCliente::SalidaCliente(Model& model): BEvent(salidaC, model) {}

SalidaCliente::~SalidaCliente() {}

void SalidaCliente::eventRoutine(Entity* who) {
    Banco& b = dynamic_cast<Banco&>(owner);
    Cliente* cliente = dynamic_cast<Cliente*>(who);
    std::cout << "El cliente numero " << cliente->nro << " de la caja " << cliente->cajaElegida + 1 << " sale del banco. Tiempo: " << b.getSimTime() << "\n";
    if (!b.qInicial.empty()) {
        // Agarro al primero de la cola
        Cliente* nuevoClienteCaja = dynamic_cast<Cliente*>(b.qInicial.pop());
        nuevoClienteCaja->cajaElegida = cliente->cajaElegida;
        std::cout << "La caja " << nuevoClienteCaja->cajaElegida + 1 << " pasa a atender el cliente numero " << nuevoClienteCaja->nro << ". Tiempo: " << b.getSimTime() << "\n\n";

        //Registro tiempo de espera en la cola que tuvo el cliente
        b.tEsperaO.log(b.getSimTime() - nuevoClienteCaja->getClock());

        b.schedule(b.distTiempoServicio.sample() + 10, nuevoClienteCaja, salidaC);
    }
    else {
        b.cajas[cliente->cajaElegida]->disponible = true;
        std::cout << "La caja " << cliente->cajaElegida + 1 << " queda disponible. Tiempo: " << b.getSimTime() << "\n\n";
    }

    // Registro datos del largo de la cola
    b.lColaCajasTW.log(b.qInicial.size());

    // Registro datos sobre la cantidad de clientes
    b.cantClientesActuales--;
	b.cantClientesTW.log(b.cantClientesActuales);

    delete who;
}




