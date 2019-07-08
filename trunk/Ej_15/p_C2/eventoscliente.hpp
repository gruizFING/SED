#ifndef EVENTOSCLIENTE_HPP_INCLUDED
#define EVENTOSCLIENTE_HPP_INCLUDED

#include <eosim/core/bevent.hpp>
#include <eosim/core/entity.hpp>
#include <string>

// identificador del evento fijo ClienteFeeder
const std::string clienteF = "ClienteFeeder";

class ClienteFeeder: public eosim::core::BEvent {
public:
	// constructor
	ClienteFeeder(eosim::core::Model& model);
	// destructor
	~ClienteFeeder();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);

	double fun(double t);

};

// identificador del evento fijo SalidaCliente
const std::string salidaC = "SalidaCliente";


class SalidaCliente: public eosim::core::BEvent {
public:
	// constructor
	SalidaCliente(eosim::core::Model& model);
	// destructor
	~SalidaCliente();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};

#endif // EVENTOSCLIENTE_HPP_INCLUDED
