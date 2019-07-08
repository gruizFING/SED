#ifndef SALA_HPP_INCLUDED
#define SALA_HPP_INCLUDED

#include <eosim/core/bevent.hpp>
#include <eosim/core/entity.hpp>
#include <string>

class Sala: public eosim::core::Entity {

    private:
        bool abierta;
        bool libre;

    public:
        Sala(bool a, bool l);
        bool estaLibre(){ return libre; }
        bool estaAbierta(){ return abierta; }
        void cerrar(){ abierta = false; }
        void abrir(){ abierta = true; }
        void ocupar(){ libre = false; }
        void liberar(){ libre = true; }
};

const std::string finOS = "FinalizarOperacionSala";

class FinalizarOperacionSala: public eosim::core::BEvent {
public:
	// constructor
	FinalizarOperacionSala(eosim::core::Model& model);
	// destructor
	~FinalizarOperacionSala();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};


const std::string abrirSala = "AbrirSala";

class AbrirSala: public eosim::core::BEvent {
public:
	// constructor
	AbrirSala(eosim::core::Model& model);
	// destructor
	~AbrirSala();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};

const std::string cerrarSala = "CerrarSala";

class CerrarSala: public eosim::core::BEvent {
public:
	// constructor
	CerrarSala(eosim::core::Model& model);
	// destructor
	~CerrarSala();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};

#endif // SALA_HPP_INCLUDED

