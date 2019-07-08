#include "Comprador.h"


bool Comparador::compare(Paciente* p1, Paciente* p2)
{
    if(p1.getPrioridad() < p2.getPrioridad())
        return true;
    else
        return false;
}
