#ifndef CMAM_HPP_
#define CMAM_HPP_

#include <eosim/core/bevent.hpp>
#include <eosim/core/entity.hpp>
#include <string>


// identificador del evento fijo CMAM
const std::string CMAMR = "CMAMRegister";

class CMAMRegister: public eosim::core::BEvent {
public:
	// constructor
	CMAMRegister(eosim::core::Model& model);
	// destructor
	~CMAMRegister();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};






#endif
