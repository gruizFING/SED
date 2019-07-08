#include "eventoscliente.hpp"
#include "banco.hpp"
#include "cliente.hpp"
#include <iostream>
#include <math.h>

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

        //Registro datos de largo de cola
        unsigned int largoCola = b.qInicial.size();
        b.lColaCajasTW.log(largoCola);
        if (largoCola > b.largoFilaMax) {
            b.largoFilaMax = largoCola;
            b.dispActualizarLargoMaxColas();
        }

        cliente->spr.setMoves(move(b.filaEspera.first - (largoCola - 1) * 48, b.filaEspera.second, 0));
    }
    else // Poner al cliente en la caja elegida para ser atendido
    {
        cliente->cajaElegida = numeroCaja;
        Caja* caja = b.cajas[numeroCaja];
        caja->disponible = false;
        std::cout << "La caja " << numeroCaja + 1 << " atiende al cliente numero " << cliente->nro << ". Tiempo: " << b.getSimTime() << "\n\n";

        //Se toma el tiempo de ir a la caja en 10 seg agregandolo al tiempo de servicio
        double stay = 0.0;
        if (cliente->retiraDinero)
            stay = b.distTiempoServicioR.sample() + 10;
        else
            stay = b.distTiempoServicioNR.sample() + 10;
        b.schedule(stay, cliente, salidaC);

        //Registro tiempo de espera para este cliente igual a 0
        b.tEsperaO.log(0);

        cliente->spr.setMoves(move(b.filaEspera.first, b.filaEspera.second, stay*0.05).move(caja->pos.first - 64, caja->pos.second, 10));
    }

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
        double tiempoEspera = b.getSimTime() - nuevoClienteCaja->getClock();
        b.tEsperaO.log(tiempoEspera);
        unsigned int tiempoEsperaInt = (unsigned int)(tiempoEspera + 0.5);
        if (tiempoEsperaInt > b.tiempoEsperaMax) {
            b.tiempoEsperaMax = tiempoEsperaInt;
            b.dispActualizarEsperaMax();
        }

        //Se toma el tiempo de ir a la caja en 10 seg agregandolo al tiempo de servicio
        double stay = 0.0;
        if (nuevoClienteCaja->retiraDinero)
            stay = b.distTiempoServicioR.sample() + 10;
        else
            stay = b.distTiempoServicioNR.sample() + 10;
        b.schedule(stay, nuevoClienteCaja, salidaC);

        Caja* caja = b.cajas[cliente->cajaElegida];
        nuevoClienteCaja->spr.setMoves(move(caja->pos.first - 64, caja->pos.second, 10));
        // mover a todos para adelante
		for (unsigned int i = 0; i < b.qInicial.size(); i++) {
            Cliente* tempClient = dynamic_cast<Cliente*>(b.qInicial[i]);
			tempClient->spr.setMoves(move(b.filaEspera.first - i * 48, b.filaEspera.second, 0));
		}

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
    b.dispActualizarClientesEnSistema();

    cliente->spr.setMoves(move(b.puertaSalida.first, b.puertaSalida.second, 10).move(b.salida.first, b.salida.second, 5));
    delete who;
}




