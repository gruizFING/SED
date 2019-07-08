#include "CMAM.hpp"
#include "hospitalsimple.hpp"
#include <iostream>
#include "constantes.hpp"
using namespace eosim::core;


CMAMRegister::CMAMRegister(Model& model): BEvent(CMAMR, model) {}

CMAMRegister::~CMAMRegister() {}

void CMAMRegister::eventRoutine(Entity* who) {
	HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);

    //double mean = h.lCola.getMean();
    //std::cout << mean << " + " << h.pAcumAnterior << " / " << h.N << std::endl;
	//h.pAcumulados.log((mean + h.pAcumAnterior)/h.N);
	h.pAcumulados.log(h.lCola.getMean());
    //h.N++;
    //h.pAcumAnterior += mean;

	h.schedule(timeIntervalCMAM, new Entity(), CMAMR);
}
