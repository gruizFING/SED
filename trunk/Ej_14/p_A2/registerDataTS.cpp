#include "registerDataTS.hpp"
#include "banco.hpp"
#include <iostream>
#include "constantes.hpp"
using namespace eosim::core;


RegisterDataTS::RegisterDataTS(Model& model): BEvent(RDTS, model) {}

RegisterDataTS::~RegisterDataTS() {}

void RegisterDataTS::eventRoutine(Entity* who) {
    Banco& b = dynamic_cast<Banco&>(owner);

    double tEsperaPromedio = b.tEsperaO.getMean();
    double lColasPromedio = b.lColaCajasTW.getMean();
    b.tEsperaMeanTS.log(tEsperaPromedio);
    b.cantClientesTS.log(b.cantClientesTW.getMean());
    b.lColaCajasMeanTS.log(lColasPromedio);

    unsigned int tEsperaPromedioInt = (unsigned int)(tEsperaPromedio + 0.5);
    unsigned int lColasPromedioInt = (unsigned int)(lColasPromedio + 0.5);
    b.dispActualizarPromedios(lColasPromedioInt, tEsperaPromedioInt);

	b.schedule(timeIntervalRDTSseg, new Entity(), RDTS);
	delete who;
}
