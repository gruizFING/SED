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
    std::string tipo = c->retiraDinero ? "Retiro" : "Otro";
    std::cout << "Llega el cliente numero " << c->nro << " de tipo '" << tipo << "'. Tiempo: " << who->getClock() << "\n";

	// se castea owner a un Banco
	Banco& b = dynamic_cast<Banco&>(owner);
	b.q0.push(who);
	b.schedule(0, who, elegirC);

    // Regristro datos sobre la cantidad de clientes
	b.cantClientesActuales++;
	b.cantClientesTW.log(b.cantClientesActuales);

    // creo proximo cliente y se agenda el arribo del nuevo cliente
    // 1 de cada 3 son retiro
    double probRetiro = b.distProbRetiro.sample();
	Cliente* nuevoCliente = new Cliente(probRetiro > ((double)1/3) ? false : true);
	nuevoCliente->nro = ++b.nroCliente;
	//b.schedule(b.distArribosClientes.sample(), nuevoCliente, clienteF);

	//Agendo proximo con el thinning
	double t, u, m,result;
	m = maxRateArribos;
	t = b.getSimTime();
	do
	{
	    //La distribucion de arribos es nexexp con media 1/m
        u = b.distArribosClientes.sample();
		t = t + u;

		double rnd = b.distProbThinning.sample();
		double f = fun(t);
		if (f == -1){
			result =  -1;
			break;
		}
		if(rnd < f/m){
			result = t -  b.getSimTime();
			break;
		}
	} while (true);
	if (result == -1){
        //std::cout << "result -1" << "\n"; Error
	}else{
        b.schedule(result, nuevoCliente, clienteF);
	}
}

double ClienteFeeder::fun(double t)
{
	if (t < 3600.0)
		return arrayRateArribos[0];
	else if(t < 2.0*3600.0)
		return arrayRateArribos[1];
	else if(t < 3.0*3600.0)
		return arrayRateArribos[2];
	else if (t < 4.0*3600.0)
		return arrayRateArribos[3];
	else if (t < 5.0*3600.0)
        return arrayRateArribos[4];
    else if (t < 6.0*3600.0)
        return arrayRateArribos[5];
    else
		return -1;
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
        if (caja->soloRetiro)
            b.schedule(b.distTiempoServicioR.sample(), cliente, salidaC);
        else
            b.schedule(b.distTiempoServicioNR.sample(), cliente, salidaC);
        //Registro largo de cola 1 en el caso que solo 1 siendo atendido..
        b.lColaCajasTW.log(1);
        //Registro tiempo de espera para este cliente 0..
        b.tEsperaO.log(0);
    }
    else {
        // pongo al cliente en la fila de la caja elegida
        b.filasCajas[numeroFila]->push(cliente);
        std::cout << "El cliente numero " << cliente->nro << " pasa a esperar en la fila de la caja " << numeroFila + 1 <<  ". Tiempo: " << b.getSimTime() << "\n\n";
        //Registro datos de largo de cola
        b.lColaCajasTW.log(b.filasCajas[numeroFila]->size());
    }
}

// en el constructor se utiliza el identificador definido en eventoscliente.hpp
SalidaCliente::SalidaCliente(Model& model): BEvent(salidaC, model) {}

SalidaCliente::~SalidaCliente() {}

void SalidaCliente::eventRoutine(Entity* who) {
    Banco& b = dynamic_cast<Banco&>(owner);
    Cliente* cliente = dynamic_cast<Cliente*>(who);

    //std::cout << "El cliente numero " << cliente->nro << " de la caja " << cliente->cajaElegida + 1 << " sale del banco. Tiempo: " << b.getSimTime() << "\n";
    if (!b.filasCajas[cliente->cajaElegida]->empty()) {
        // Agarro al primero de la cola
        Cliente* nuevoClienteCaja = dynamic_cast<Cliente*>(b.filasCajas[cliente->cajaElegida]->pop());
        std::cout << "La caja " << nuevoClienteCaja->cajaElegida + 1 << " pasa a atender el cliente numero " << nuevoClienteCaja->nro << ". Tiempo: " << b.getSimTime() << "\n\n";

        //Registro tiempo de espera en la cola que tuvo el cliente
        b.tEsperaO.log(b.getSimTime() - nuevoClienteCaja->getClock());

        Caja* caja = b.cajas[cliente->cajaElegida];
        if (caja->soloRetiro)
            b.schedule(b.distTiempoServicioR.sample(), nuevoClienteCaja, salidaC);
        else
            b.schedule(b.distTiempoServicioNR.sample(), nuevoClienteCaja, salidaC);
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

    delete who;
}

// en el constructor se utiliza el identificador definido en eventoscliente.hpp
QueueSwopping::QueueSwopping(Model& model): BEvent(queueC, model) {}

QueueSwopping::~QueueSwopping() {}

void QueueSwopping::eventRoutine(Entity* who) {
    Banco& b = dynamic_cast<Banco&>(owner);
    double sizeA = b.filasCajas[0]->size();
    double sizeB = b.filasCajas[2]->size();
    double sizeC = b.filasCajas[3]->size();
    Cliente* cliente;
    if ((sizeA == 0) or (sizeB == 0) or (sizeC == 0)){
        if (sizeA == 0){
            if (sizeB > sizeC){

                for (double i = sizeB; i > sizeB/2; i--){
                   cliente = dynamic_cast<Cliente*>(b.filasCajas[2]->pop());
                   b.cambiarFila(cliente, 0);
                }
            }else{
                for (double i = sizeC; i > sizeC/2; i--){
                    cliente = dynamic_cast<Cliente*>(b.filasCajas[3]->pop());
                    b.cambiarFila(cliente, 0);
                }
            }
        }else if (sizeB == 0){
            if (sizeA > sizeC){
                for (double i = sizeA; i > sizeA/2; i--){
                    cliente = dynamic_cast<Cliente*>(b.filasCajas[0]->pop());
                    b.cambiarFila(cliente, 2);
                }
            }else{
                for (double i = sizeC; i > sizeC/2; i--){
                    cliente = dynamic_cast<Cliente*>(b.filasCajas[3]->pop());
                    b.cambiarFila(cliente, 2);
                }
            }
        }else {
            if (sizeA > sizeB){
                for (double i = sizeA; i > sizeA/2; i--){
                    cliente = dynamic_cast<Cliente*>(b.filasCajas[0]->pop());
                    b.cambiarFila(cliente, 3);
                }
            }else{
                for (double i = sizeB; i > sizeB/2; i--){
                    cliente = dynamic_cast<Cliente*>(b.filasCajas[2]->pop());
                    b.cambiarFila(cliente, 3);
                }
            }
        }
    }

    b.schedule(timeIntervalRDTSseg, new Entity(), queueC);
    delete who;
}

/*
void QueueSwopping::eventRoutine(Entity* who) {
    Banco& b = dynamic_cast<Banco&>(owner);
    unsigned int sizeA = b.filasCajas[0]->size();
    unsigned int sizeB = b.filasCajas[2]->size();
    unsigned int sizeC = b.filasCajas[3]->size();
    int counter = 1;
    //Cola auxiliar
    eosim::utils::EntityQueueFifo q ;
    unsigned int maxAB = (sizeA<sizeB)?sizeB:sizeA;
    unsigned int maximo = (maxAB<sizeC)?sizeC:maxAB;

    Cliente* cliente;

    if ( (!((sizeA == 0) and (sizeB == 0) and (sizeC == 0)))){
        for (unsigned int i = 0 ; i < maximo ; i++){
            if (!b.filasCajas[0]->empty()){
                cliente = dynamic_cast<Cliente*>(b.filasCajas[0]->pop());
                q.push(cliente);
            }
            if (!b.filasCajas[2]->empty()){
                cliente = dynamic_cast<Cliente*>(b.filasCajas[2]->pop());
                q.push(cliente);
            }
            if (!b.filasCajas[3]->empty()){
                cliente = dynamic_cast<Cliente*>(b.filasCajas[3]->pop());
                q.push(cliente);
            }
        }
        for (unsigned int i = 0;i < q.size();i++){
            if (counter == 1){
                cliente = dynamic_cast<Cliente*>(q.pop());
                b.cambiarFila(cliente, 0);
            }else if (counter == 2){
                cliente = dynamic_cast<Cliente*>(q.pop());
                b.cambiarFila(cliente, 2);
            }else if (counter == 3){
                cliente = dynamic_cast<Cliente*>(q.pop());
                b.cambiarFila(cliente, 3);
            }
            counter++;
            if (counter == 4){
                counter = 1;
            }
        }
    }

    b.schedule(timeIntervalSwap, new Entity(), queueC);
    delete who;
}
*/
