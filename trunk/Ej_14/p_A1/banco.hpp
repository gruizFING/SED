#ifndef BANCO_HPP_INCLUDED
#define BANCO_HPP_INCLUDED

#include <eosim/core/model.hpp>
#include <eosim/utils/entityqueuefifo.hpp>
#include <eosim/core/renewable.hpp>
#include <eosim/dist/negexpdist.hpp>
#include <eosim/dist/lognormaldist.hpp>
#include <eosim/dist/normaldist.hpp>
#include <eosim/dist/uniformdist.hpp>
#include <eosim/statics/timeweighted.hpp>
#include <eosim/statics/observation.hpp>
#include <eosim/dist/mt19937.hpp>
#include <eosim/statics/timeseries.hpp>
#include <eosim/statics/timeweighted.hpp>
#include <eosim/statics/observation.hpp>
#include "cliente.hpp"
#include "eventoscliente.hpp"
#include "caja.hpp"
#include "registerDataTS.hpp"
#include "constantes.hpp"

/*
Este es el modelo de "4 colas" simulacion del ej 3.9.4 del banco presentado en el libro
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
	// evento de registro de datos para los time series
    RegisterDataTS rdTS;
    // evento de condicionado para el dibujado
	eosim::graphic::RefreshGraphic rG;

public:

    // Display del modelo
	eosim::graphic::Display disp;

	// constantes graficas
	std::vector<std::pair<double, double> > filasEspera;
	const std::pair<double, double> puertaEntrada;
	const std::pair<double, double> puertaSalida;
	const std::pair<double, double> salida;
	const std::pair<double, double> salidaDatos;

	// TextSprite para clientes del sistema
    eosim::graphic::TextSprite* sprMaxClientes;
    eosim::graphic::TextSprite* sprClientes;

    // TextSprite para tiempos del sistema
    eosim::graphic::TextSprite* sprMaxTiempoEspera;
    eosim::graphic::TextSprite* sprTiempoEsperaPromedio;

    // TextSprite para largos del sistema
    eosim::graphic::TextSprite* sprMaxLargo;
    eosim::graphic::TextSprite* sprLargoPromedio;

    // cant de cajas del banco
    unsigned int cantCajas;
    // cantidad de clientes en el banco actualmente en la simulacion
    unsigned int cantClientesActuales;
     // cantidad de clientes maximo en el banco
    unsigned int cantClientesMax;
     // cantidad de clientes maximo en el banco
    unsigned int largoFilaMax;
     // cantidad de clientes maximo en el banco
    unsigned int tiempoEsperaMax;
    // numero de cliente, contador de clientes entrados al sistema
    unsigned int nroCliente;

    // Distribuciones para las duraciones de actividades
    // distribucion exponencial negativa de arribo de clientes
	eosim::dist::NegexpDist distArribosClientes;
	// distribucion LogNormal para el tiempo de servicio para las cajas de no solo retiro
	eosim::dist::LogNormalDist distTiempoServicioNR;
	// distribucion LogNormal para el tiempo de servicio para las cajas solo retiro
	eosim::dist::LogNormalDist distTiempoServicioR;
	// distribucion Uniforme [0,1] para la probabilidad de ser cliente tipo retiro o no
	eosim::dist::UniformDist distProbRetiro;

	//Recoleccion de datos
    // histograma timeseries para registro de cantidad de clientes
    eosim::statics::TimeSeries cantClientesTS;
    // histograma timeseries para registro de media de tiempo de espera
    eosim::statics::TimeSeries tEsperaMeanTS;
	// histograma timeseries para registro de las media de los largos de las colas
    eosim::statics::TimeSeries lColaCajasMeanTS;

    // acumulador de datos sobre los tiempos de espera de los clientes en las colas
	eosim::statics::Observation tEsperaO;
	// acumulador de datos sobre los tiempos de espera de los clientes en las colas
	//eosim::statics::Observation tServicioO;
	// acumulador de datos sobre la cantidad de clientes simultaneamente en el banco
	eosim::statics::TimeWeighted cantClientesTW;
	// acumulador de datos sobre el largo de la colas para las cajas
	eosim::statics::TimeWeighted lColaCajasTW;

	// cola de espera para elegir fila
	eosim::utils::EntityQueueFifo q0;

	// vector con las colas de espera para cada caja
    std::vector<eosim::utils::EntityQueueFifo*> filasCajas;
    // vector con las cajas
    std::vector<Caja*> cajas;

	// constructor del modelo
	Banco(unsigned int cantCajas, bool cajasSoloRetiro[], double tiempoEntreArribos, double tiempoServicioM1,
                double tiempoServicioM2, double tiempoServicioS1, double tiempoServicioS2);
	// destructor del modelo
	~Banco();
	// inicializa y registra los atributos del modelo, operacion abstracta de eosim::core::Model
	void init();
	// lleva al modelo a su estado inicial, operacion abstracta de eosim::core::Model
	void doInitialSchedules();

	void dispActualizarClientesEnSistema();
	void dispActualizarLargoMaxColas();
	void dispActualizarEsperaMax();
	void dispActualizarPromedios(unsigned int largoColasP, unsigned int tEsperaPromedio);
};

#endif // BANCO_HPP_INCLUDED
