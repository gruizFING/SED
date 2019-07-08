#include "registerDataTS.hpp"
#include "banco.hpp"
#include <iostream>
#include "constantes.hpp"
using namespace eosim::core;


RegisterDataTS::RegisterDataTS(Model& model): BEvent(RDTS, model) {}

RegisterDataTS::~RegisterDataTS() {}

void RegisterDataTS::eventRoutine(Entity* who) {
    Banco& b = dynamic_cast<Banco&>(owner);

    b.tEsperaMeanTS.log(b.tEsperaO.getMean());
    b.cantClientesTS.log(b.cantClientesTW.getMean());
    b.lColaCajasMeanTS.log(b.lColaCajasTW.getMean());

	b.schedule(timeIntervalRDTSseg, new Entity(), RDTS);
	delete who;
}
