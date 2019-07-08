#include "logHospital.hpp"
#include "hospitalsimple.hpp"
#include <iostream>
#include <cstdlib>
using namespace eosim::core;

// en el constructor se utiliza el identificador definido en paciente.hpp
// en el constructor se utiliza el identificador definido en pacientefeeder.hpp
LogHospital::LogHospitalInit(Model& model): BEvent(logH, model) {}

LogHospital::~LogHospitalInit() {}

void LogHospital::eventRoutine(Entity* who) {


}
