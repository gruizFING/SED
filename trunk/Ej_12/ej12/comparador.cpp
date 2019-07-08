#include "comparador.h"
#include "paciente.hpp"
#include <iostream>

using namespace eosim::core;

Comparador::Comparador() {}

Comparador::~Comparador() {}

bool Comparador::compare(Entity* e1, Entity* e2)
{
    Paciente* p1 = dynamic_cast<Paciente*>(e1);
    Paciente* p2 = dynamic_cast<Paciente*>(e2);

    if(p1->getPrioridad() < p2->getPrioridad())
        return true;
    else
        return false;
}
