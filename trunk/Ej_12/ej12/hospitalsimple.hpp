#ifndef HOSPITALSIMPLE_HPP_INCLUDED
#define HOSPITALSIMPLE_HPP_INCLUDED

#include <eosim/core/model.hpp>
#include <eosim/utils/entityqueuefifo.hpp>
#include <eosim/core/renewable.hpp>
#include <eosim/dist/negexpdist.hpp>
#include <eosim/dist/uniformdist.hpp>
#include <eosim/utils/entityqueueordered.hpp>
#include <eosim/utils/entityqueuefifo.hpp>
#include <eosim/dist/lognormaldist.hpp>
#include <eosim/statics/timeseries.hpp>
#include <eosim/statics/timeweighted.hpp>
#include <eosim/statics/observation.hpp>
#include "paciente.hpp"
#include "loghospital.hpp"
#include "comparador.h"
#include "Sala.hpp"


class IniciarEstadiaPostOperacion: public eosim::core::CEvent {

    public:
        // constructores
        IniciarEstadiaPostOperacion(eosim::core::Model& model);
        // destructor
        ~IniciarEstadiaPostOperacion();
        // rutina del evento fijo
        void eventRoutine();
};

class IngresarPaciente: public eosim::core::CEvent {

    public:
        // constructores
        IngresarPaciente(eosim::core::Model& model);
        // destructor
        ~IngresarPaciente();
        // rutina del evento fijo
        void eventRoutine();
};

class IniciarOperacion: public eosim::core::CEvent {

    public:
        // constructores
        IniciarOperacion(eosim::core::Model& model);
        // destructor
        ~IniciarOperacion();
        // rutina del evento fijo
        void eventRoutine();
};

class HospitalSimple: public eosim::core::Model {
private:
    int cantCamas;
    // evento de arribo de los pacientes y alimentador (fijo)
	PacienteFeeder pF;
	// evento de salida de los pacientes (fijo)
	SalidaPaciente sP;
	//
    LogHospital logH;
    //
    IngresarPaciente ingP;
    //
    FinEtapaPreoperatoria finEtapaP;
    //
    IniciarOperacion iniOp;
    //
    FinalizarOperacionSala finOperSala;
    //
    AbrirSala aS;
    //
    CerrarSala cS;
    //
    FinalizarOperacion finOpPaciente;
    //
    IniciarEstadiaPostOperacion iniPostOper;

public:
    // variable utilizada para determinar cuando el sistema finaliza su período run-in
    bool estable;
    // distribución utilizada para determinar el tipo de paciente que ingresa al hospital
    eosim::dist::UniformDist uniforme;
	// distribucion aleatoria de arribos de pacientes (exponencial)
	eosim::dist::NegexpDist arribos;
	// distribucion aleatoria de estadia de pacientes (log Normal)
	eosim::dist::LogNormalDist estadia;
    // distribucion aleatoria de estadia de pacientes antes de la operacion (log Normal)
	eosim::dist::LogNormalDist estadiaPreOperacion;
    // distribucion aleatoria de estadia de pacientes luego de la operacion (log Normal)
	eosim::dist::LogNormalDist estadiaPostOperacion;
	// acumulador de datos sobre los tiempos de espera en las colas
	eosim::statics::Observation esperaOperacion;
	// distribucion aleatoria de estadia de pacientes (log Normal)
	eosim::dist::LogNormalDist duracionOperacion;
	// cola de espera de pacientes que sí se operan
	eosim::utils::EntityQueueOrdered colaOperacion;
	// cola de espera de pacientes que no se operan
	eosim::utils::EntityQueueOrdered colaNoOperacion;
	// cola de espera de pacientes que sí se operan
	eosim::utils::EntityQueueFifo colaOperar;
	// cola de espera de pacientes que sí se operan
	eosim::utils::EntityQueueFifo colaPostOperacion;
	// camas del hospital
	eosim::core::Renewable camas;
	// acumulador de datos sobre el largo medio de la cola de pacientes
	eosim::statics::TimeWeighted lColaCama;
	// sala de operacion
	Sala s;
	// constructor del modelo
	HospitalSimple(int cant);
	// destructor del modelo
	~HospitalSimple();
	// inicializa y registra los atributos del modelo, operacion abstracta de eosim::core::Model
	void init();
	// lleva al modelo a su estado inicial, operacion abstracta de eosim::core::Model
	void doInitialSchedules();
};

#endif // HOSPITALSIMPLE_HPP_INCLUDED
