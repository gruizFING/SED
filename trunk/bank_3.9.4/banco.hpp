#ifndef BANCO_HPP_INCLUDED
#define BANCO_HPP_INCLUDED

#include <eosim/core/model.hpp>
#include <eosim/utils/entityqueuefifo.hpp>
#include <eosim/core/renewable.hpp>
#include <eosim/dist/negexpdist.hpp>
#include <eosim/statics/timeweighted.hpp>
#include <eosim/statics/observation.hpp>
#include <eosim/dist/mt19937.hpp>
#include "cliente.hpp"
#include "eventoscliente.hpp"
#include "caja.hpp"

/*
Este es el modelo de simulacion del ej 3.9.4 del banco presentado en el libro
Simulation Modelling with Pascal - Ruth Davies, Robert O'Keefe

Este modelo esta construido en 2 fases
*/

class Banco: public eosim::core::Model {
private:
	// evento de arribo de los clientes y alimentador (fijo)
	ClienteFeeder cF;
	// evento de salida de los clientes (fijo)
	SalidaCliente cS;
	// evento de eleccion de fila para los clientes (fijo)
	elegirCajaCliente eC;

public:
    // tasa de arribos de los clientes
	double tasaArribos;
	// tiempo de servicio de los clientes
	double tiempoServicioCliente;
	// cant de cajas del banco
	unsigned int cantCajas;

    // numero de cliente
    unsigned int nroCliente;
    // random generator
    eosim::dist::Mt19937 numberGen;

	// cola de espera para elegir fila
	eosim::utils::EntityQueueFifo q0;

	// vector con las colas de espera para cada caja
    std::vector<eosim::utils::EntityQueueFifo*> filasCajas;
    // vector con las cajas
    std::vector<Caja*> cajas;

	// acumulador de datos sobre los tiempos de espera en las colas
	eosim::statics::Observation tEspera;
	// acumulador de datos sobre el largo medio de la cola
	eosim::statics::TimeWeighted lCola;
	// constructor del modelo
	Banco(unsigned int cantCajas, bool cajasSoloRetiro[], double tasaArribos, double tiempoServicio);
	// destructor del modelo
	~Banco();
	// inicializa y registra los atributos del modelo, operacion abstracta de eosim::core::Model
	void init();
	// lleva al modelo a su estado inicial, operacion abstracta de eosim::core::Model
	void doInitialSchedules();
};

#endif // BANCO_HPP_INCLUDED
