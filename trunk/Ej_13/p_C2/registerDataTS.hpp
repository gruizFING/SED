#ifndef REGISTERDATATS_HPP_INCLUDED
#define REGISTERDATATS_HPP_INCLUDED

#include <eosim/core/bevent.hpp>
#include <eosim/core/entity.hpp>
#include <string>

// identificador del evento fijo RDTS
const std::string RDTS = "RegisterDataTS";

class RegisterDataTS: public eosim::core::BEvent {
public:
	// constructor
	RegisterDataTS(eosim::core::Model& model);
	// destructor
	~RegisterDataTS();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};


#endif // REGISTERDATATS_HPP_INCLUDED
