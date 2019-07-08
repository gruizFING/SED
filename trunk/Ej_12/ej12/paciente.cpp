#include "paciente.hpp"
#include "hospitalsimple.hpp"
#include <iostream>
#include <cstdlib>

using namespace eosim::core;

int Paciente::getPrioridad(){ return prioridad; }

void Paciente::setPrioridad(int p) { prioridad = p; }

int Paciente::getIniOperacion() { return iniOperacion; }

void Paciente::setIniOperacion(unsigned int ini) { iniOperacion = ini; }

PacienteFeeder::PacienteFeeder(Model& model): BEvent(pacienteF, model) {}

PacienteFeeder::~PacienteFeeder() {}

void PacienteFeeder::eventRoutine(Entity* who) {

	std::cout << "llego un paciente en " << who->getClock() << "\n";

	HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);
    Paciente* p = (Paciente*)who;

    double u = h.uniforme.sample();

    p->setPrioridad(rand() % 4 + 1);

    h.lColaCama.log(h.colaNoOperacion.size()+h.colaOperacion.size()+h.colaPostOperacion.size());

    if(u < 0.3)
    {
        h.colaNoOperacion.push(who);
    }
    else
    {
        h.colaOperacion.push(who);
    }

	h.schedule(h.arribos.sample(), new Paciente(), pacienteF);
}


SalidaPaciente::SalidaPaciente(Model& model): BEvent(salidaP, model) {}

SalidaPaciente::~SalidaPaciente() {}

void SalidaPaciente::eventRoutine(Entity* who) {

	std::cout << "un paciente se retira en " << who->getClock() << "\n";

	HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);
	h.camas.returnBin(1);

	delete who;
}


FinEtapaPreoperatoria::FinEtapaPreoperatoria(Model& model): BEvent(finEP, model) {}

FinEtapaPreoperatoria::~FinEtapaPreoperatoria() {}

void FinEtapaPreoperatoria::eventRoutine(Entity* who) {

    std::cout << "un paciente esta listo para operarse " << who->getClock() << "\n";

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);

    Paciente* p = (Paciente*)who;
    p->setIniOperacion(h.getSimTime());

    h.camas.returnBin(1);
	h.colaOperar.push(who);
}


FinalizarOperacion::FinalizarOperacion(Model& model): BEvent(finOp, model) {}

FinalizarOperacion::~FinalizarOperacion() {}

void FinalizarOperacion::eventRoutine(Entity* who) {

    std::cout << "un paciente ha finalizado su operacion" << who->getClock() << "\n";

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);
    h.lColaCama.log(h.colaNoOperacion.size()+h.colaOperacion.size()+h.colaPostOperacion.size());
    h.colaPostOperacion.push(who);
}
