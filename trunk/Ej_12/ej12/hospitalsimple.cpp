#include "hospitalsimple.hpp"
#include "comparador.h"
#include "constantes.hpp"
#include <eosim/core/entity.hpp>
#include <eosim/dist/numbergenerator.hpp>
#include <eosim/statics/timeseries.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib>


using namespace eosim::core;
using namespace eosim::dist;

using namespace std;

HospitalSimple::HospitalSimple(int cant):
    cantCamas(cant),
    pF(*this),
    sP(*this),
    logH(*this),
    ingP(*this),
    finEtapaP(*this),
    iniOp(*this),
    finOperSala(*this),
    aS(*this),
    cS(*this),
    finOpPaciente(*this),
    iniPostOper(*this),
    estable(false),
    uniforme(MT19937, 0, 1),
    arribos(MT19937, tasaArribosNoQuirugircos + tasaArribosQuirugircos),
    estadia(MT19937, mediaTiempoEstadiaC,desviacionTiempoEstadiaC),
    estadiaPreOperacion(MT19937,mediaEstadiaPreOperacion,desviacionEstadiaPreOperacion),
    estadiaPostOperacion(MT19937,mediaEstadiaPostOperacion,desviacionEstadiaPostOperacion),
    esperaOperacion("Tiempos de espera para operarse"),
	duracionOperacion(MT19937, mediaTiempoOperacion, desviacionTiempoOperacion),
    colaOperacion(new Comparador()),
    colaNoOperacion(new Comparador()),
	colaOperar(),
	colaPostOperacion(),
	camas(cant, cant),
    lColaCama("Suma de los largos de las colas para obtener una cama", *this),
    s(false,true)
	{}

HospitalSimple::~HospitalSimple() {}

void HospitalSimple::init() {
	// registro los eventos B
	registerBEvent(&pF);
	registerBEvent(&sP);
    registerBEvent(&logH);
    registerBEvent(&finEtapaP);
    registerBEvent(&finOperSala);
    registerBEvent(&aS);
    registerBEvent(&cS);
    registerBEvent(&finOpPaciente);
    // registro los eventos C
    registerCEvent(&iniOp);
    registerCEvent(&ingP);
    registerCEvent(&iniPostOper);
	// registro las distribuciones
	registerDist(&arribos);
	registerDist(&estadia);
	registerDist(&estadiaPreOperacion);
	registerDist(&estadiaPostOperacion);
	registerDist(&duracionOperacion);
}

void HospitalSimple::doInitialSchedules() {

    schedule (0.0, &this->s, abrirSala );
	schedule(0.0, new Entity(), lH);
	schedule(100.0, new Paciente(), pacienteF);
}

IngresarPaciente::IngresarPaciente(eosim::core::Model& model): CEvent(model) {}

IngresarPaciente::~IngresarPaciente() {}

void IngresarPaciente::eventRoutine() {

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);

    while(h.camas.isAvailable(1))
    {
        if(!h.colaNoOperacion.empty())
        {
            h.camas.acquire(1);
            std::cout << "un paciente fue aceptado en una cama " << h.getSimTime() << "\n";
            Entity* p = h.colaNoOperacion.pop();
            h.schedule(h.estadia.sample(), p, salidaP);
        }
        else if(!h.colaOperacion.empty())
        {
            h.camas.acquire(1);
            std::cout << "un paciente fue aceptado en una cama " << h.getSimTime() << "\n";
            Entity* p = h.colaOperacion.pop();
            h.schedule(h.estadiaPreOperacion.sample(), p, finEP);
        }
        else
        {
            return;
        }
	}
}

IniciarOperacion::IniciarOperacion(eosim::core::Model& model): CEvent(model) {}

IniciarOperacion::~IniciarOperacion() {}

void IniciarOperacion::eventRoutine() {

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);

    if(h.s.estaAbierta() && h.s.estaLibre() && !h.colaOperar.empty())
    {
        std::cout << "se ha iniciado una operacion " << h.getSimTime() << "\n";

        h.s.ocupar();

        //variable para setear random o no
        bool esRandom = false;

        Paciente* p ;
        if (esRandom){
            int cantidad = h.colaOperar.size();
            int operar;
            cout << "Largo Cola"<< cantidad << "\n";
            operar = (0 + rand() % cantidad);
            cout << operar << "\n"; //mostramos por pantalla los numeros generados de 0 a cantidad


            p =(Paciente*) h.colaOperar[operar];
            h.colaOperar.remove(operar);
        }else {
            p =(Paciente*) h.colaOperar.pop();
        }


        h.esperaOperacion.log(h.getSimTime() - p->getIniOperacion());

        unsigned int duracion = h.duracionOperacion.sample();

        h.schedule(duracion,p,finOp);
        h.schedule(duracion,&h.s,finOS);
    }
}

IniciarEstadiaPostOperacion::IniciarEstadiaPostOperacion(eosim::core::Model& model): CEvent(model) {}

IniciarEstadiaPostOperacion::~IniciarEstadiaPostOperacion() {}

void IniciarEstadiaPostOperacion::eventRoutine() {

    HospitalSimple& h = dynamic_cast<HospitalSimple&>(owner);

    while(h.camas.isAvailable(1))
    {
        if(!h.colaPostOperacion.empty())
        {
            h.camas.acquire(1);
            std::cout << "un paciente fue aceptado en una cama " << h.getSimTime() << "\n";
            Entity* p = h.colaPostOperacion.pop();
            h.schedule(h.estadiaPostOperacion.sample(), p, salidaP);
        }
        else
            return;
	}
}
