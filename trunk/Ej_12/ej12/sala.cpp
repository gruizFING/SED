#include "sala.hpp"
#include "constantes.hpp"
#include "hospitalsimple.hpp"

using namespace eosim::core;

Sala::Sala(bool a, bool l):
abierta(a),
libre(l)
{}


FinalizarOperacionSala::FinalizarOperacionSala(Model& model): BEvent(finOS, model) {}

FinalizarOperacionSala::~FinalizarOperacionSala() {}

void FinalizarOperacionSala::eventRoutine(Entity* who) {

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);
    h.s.liberar();
}


AbrirSala::AbrirSala(Model& model): BEvent(abrirSala, model) {}

AbrirSala::~AbrirSala() {}

void AbrirSala::eventRoutine(Entity* who) {

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);
    h.s.abrir();
    h.schedule(tiempo_sala_abierta,who,cerrarSala);
}


CerrarSala::CerrarSala(Model& model): BEvent(cerrarSala, model) {}

CerrarSala::~CerrarSala() {}

void CerrarSala::eventRoutine(Entity* who) {

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);
    h.s.cerrar();
    h.schedule(tiempo_sala_cerrada,who,abrirSala);
}
