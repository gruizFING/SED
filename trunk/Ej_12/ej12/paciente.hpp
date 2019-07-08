#ifndef PACIENTEDOS_HPP_INCLUDED
#define PACIENTEDOS_HPP_INCLUDED

#include <eosim/core/bevent.hpp>
#include <eosim/core/entity.hpp>
#include <string>

class Paciente: public eosim::core::Entity {

    private:
        int prioridad;
        unsigned int iniOperacion;

    public:
        int getPrioridad();
        void setPrioridad(int p);
        int getIniOperacion();
        void setIniOperacion(unsigned int ini);
};

// identificador del evento fijo PacienteFeeder
const std::string pacienteF = "PacienteFeeder";

class PacienteFeeder: public eosim::core::BEvent {
public:
	// constructor
	PacienteFeeder(eosim::core::Model& model);
	// destructor
	~PacienteFeeder();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};

// identificador del evento fijo SalidaPaciente
const std::string salidaP = "SalidaPaciente";


class SalidaPaciente: public eosim::core::BEvent {
public:
	// constructor
	SalidaPaciente(eosim::core::Model& model);
	// destructor
	~SalidaPaciente();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};

const std::string finEP = "FinEtapaPreoperatoria";

class FinEtapaPreoperatoria: public eosim::core::BEvent {
public:
	// constructor
	FinEtapaPreoperatoria(eosim::core::Model& model);
	// destructor
	~FinEtapaPreoperatoria();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};

const std::string finOp = "FinalizarOperacion";

class FinalizarOperacion: public eosim::core::BEvent {
public:
	// constructor
	FinalizarOperacion(eosim::core::Model& model);
	// destructor
	~FinalizarOperacion();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};



#endif // PACIENTEUNO_HPP_INCLUDED
