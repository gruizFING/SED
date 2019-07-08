#include "hospitalsimple.hpp"
#include <eosim/core/entity.hpp>
#include <eosim/dist/numbergenerator.hpp>
#include <iostream>
#include "constantes.hpp"

using namespace eosim::core;
using namespace eosim::dist;

using namespace std;

HospitalSimple::HospitalSimple(unsigned int cantCamas, double tasaArribos, double mediaEstadia, double desviacionEstadia):
// se contruyen los eventos B, los eventos C, las distribuciones, los recursos y los histogramas
                                N(1),
                                pAcumAnterior(0.0),
								tasaArribos(tasaArribos),
								mediaEstadia(mediaEstadia),
								pF(*this),
								sP(*this),
								cmamR(*this),
								arribos(MT19937, tasaArribos),
								estadia(MT19937, mediaEstadia, desviacionEstadia),
								camas(cantCamas, cantCamas),
								tEspera("Tiempos de Espera"),
								lCola("Largos Medios de Colas", *this),
								lColaTS("Largo de Cola", *this),
								pAcumulados("CMAM Promedios Acumulados", *this) {}

HospitalSimple::~HospitalSimple() {}

void HospitalSimple::init() {
	// registro los eventos B
	registerBEvent(&pF);
	registerBEvent(&sP);
	registerBEvent(&cmamR);

	// registro las distribuciones
	registerDist(&arribos);
	registerDist(&estadia);
}

void HospitalSimple::doInitialSchedules() {
	// agendo el primer paciente
	schedule(0.0, new Entity(), pacienteF);
	// agendo el primer CMAMRegister
	schedule(timeIntervalCMAM, new Entity(), CMAMR);
}

