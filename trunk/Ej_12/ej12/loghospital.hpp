#ifndef LOGHOSPITAL_HPP_INCLUDED
#define LOGHOSPITAL_HPP_INCLUDED

#include <eosim/core/bevent.hpp>
#include <eosim/core/entity.hpp>
#include <string>

//
const std::string lH = "LogHospital";

class LogHospital : public eosim::core::BEvent {
public:
	// constructor
	LogHospital(eosim::core::Model& model);
	// destructor
	~LogHospital();
	// rutina del evento fijo
	void eventRoutine(eosim::core::Entity* who);
};

#endif // LOGHOSPITAL_HPP_INCLUDED

