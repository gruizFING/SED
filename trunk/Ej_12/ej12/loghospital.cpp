#include "loghospital.hpp"
#include "hospitalsimple.hpp"
#include <iostream>
#include <cmath>
#include <string>

using namespace eosim::core;

//
double INTERVALO = 200;

LogHospital::LogHospital(Model& model): BEvent(lH, model) {}

LogHospital::~LogHospital() {}

void LogHospital::eventRoutine(Entity* who) {

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);

    if(!h.estable)
    {
        if(floor(h.getSimTime()) > 15000)
        {
            delete who;
            h.lColaCama.reset();
            h.esperaOperacion.reset();
        }
        else
            h.schedule(INTERVALO, who, lH);
    }
}
