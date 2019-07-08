#include "banco.hpp"
#include <eosim/core/entity.hpp>
#include <eosim/dist/numbergenerator.hpp>
#include <iostream>

using namespace eosim::core;
using namespace eosim::dist;
using namespace eosim::graphic;

using namespace std;

Banco::Banco(unsigned int cantC, bool cajasSoloRetiro[], double tiempoEntreArribos, double tiempoServicioM1,
                double tiempoServicioM2, double tiempoServicioS1, double tiempoServicioS2):
// se contruyen los eventos B, los eventos C, las distribuciones, los recursos y los histogramas
								cF(*this),
								cS(*this),
								rdTS(*this),
								rG(*this, disp),
								disp(800,600),
								filaEspera(xCajas - distCajaFila*2, 300),
								puertaEntrada(50,300),
                                puertaSalida(50,350),
                                salida(0,350),
                                salidaDatos(50,30),
								cantCajas(cantC),
								distArribosClientes(MT19937, tiempoEntreArribos),
								distTiempoServicioNR(MT19937, tiempoServicioM1, tiempoServicioS1),
								distTiempoServicioR(MT19937, tiempoServicioM2, tiempoServicioS2),
								distProbRetiro(MT19937, 0 , 1),
								cantClientesTS("Cantidad de clientes TS", *this),
								tEsperaMeanTS("Media de tiempos de Espera TS", *this),
								lColaCajasMeanTS("Media de largo de la cola TS", *this),
								tEsperaO("Tiempos de Espera"),
								cantClientesTW("Cantidad de clientes TW", *this),
								lColaCajasTW("Media de largo de la cola TW", *this)
    {
        this->disp.setSpeed(0.02);
        for (unsigned int i = 0; i < cantC; i++) {
            pair<double, double> position(xCajas, yCajasInicial + i*distEntreCajas);
            Caja* caja = new Caja(this->disp, position, false); // Todas las cajas no son de solo retiro
            caja->spr.setVisible(true);
            cajas.push_back(caja);
        }
    }

Banco::~Banco() {}

void Banco::init() {
	// registro los eventos B
	registerBEvent(&cF);
	registerBEvent(&cS);
    registerBEvent(&rdTS);

    // registro los eventos C
	registerCEvent(&rG);

    // registro las distribuciones
    registerDist(&distArribosClientes);
    registerDist(&distTiempoServicioNR);
    registerDist(&distTiempoServicioR);
    registerDist(&distProbRetiro);
}

void Banco::doInitialSchedules() {
	// agendo el primer cliente
	nroCliente = 1;
	cantClientesActuales = 0;
	cantClientesMax = 0;
	largoFilaMax = 0;
	tiempoEsperaMax = 0;
	double probRetiro = this->distProbRetiro.sample();
	Cliente* cliente = new Cliente(disp, puertaEntrada, probRetiro > ((double)1/3) ? false : true);
	cliente->nro = nroCliente;
	schedule(0.0, cliente, clienteF);

	// agendo el primer registro de datos para los TS
	schedule(timeIntervalRDTSseg, new Entity(), RDTS);

	TextSprite* clientes = new TextSprite(this->disp, "Cantidad Actual de Clientes: ", 15, salidaDatos.first, salidaDatos.second);
    clientes->setVisible(true);
    TextSprite* maxClientes = new TextSprite(this->disp, "Maxima Cantidad de Clientes: ", 15, salidaDatos.first, salidaDatos.second + 25);
    maxClientes->setVisible(true);

    TextSprite* largos = new TextSprite(this->disp, "Largo promedio de fila: ", 15, salidaDatos.first + 400, salidaDatos.second);
    largos->setVisible(true);
    TextSprite* maxFilaLargo = new TextSprite(this->disp, "Largo maximo de fila: ", 15, salidaDatos.first + 400, salidaDatos.second + 25);
    maxFilaLargo->setVisible(true);

    TextSprite* esperaPromedio = new TextSprite(this->disp, "Tiempo de espera promedio: ", 15, salidaDatos.first + 200, 600 - salidaDatos.second - 45);
    esperaPromedio->setVisible(true);
    TextSprite* esperaPromedioMax = new TextSprite(this->disp, "Tiempo de espera maximo: ", 15, salidaDatos.first + 200, 600 - salidaDatos.second - 20);
    esperaPromedioMax->setVisible(true);

    sprClientes = new TextSprite(this->disp, "0", 15, salidaDatos.first + 200, salidaDatos.second);
    sprClientes->setText("0");
    sprClientes->setVisible(true);
    sprMaxClientes = new TextSprite(this->disp, "0", 15, salidaDatos.first + 200, salidaDatos.second + 25);
    sprMaxClientes->setText("0");
    sprMaxClientes->setVisible(true);

    sprLargoPromedio = new TextSprite(this->disp, "0", 15, salidaDatos.first + 600, salidaDatos.second);
    sprLargoPromedio->setText("0");
    sprLargoPromedio->setVisible(true);
    sprMaxLargo = new TextSprite(this->disp, "0", 15, salidaDatos.first + 600, salidaDatos.second + 25);
    sprMaxLargo->setText("0");
    sprMaxLargo->setVisible(true);

    sprTiempoEsperaPromedio = new TextSprite(this->disp, "0", 15, salidaDatos.first + 400, 600 - salidaDatos.second - 45);
    sprTiempoEsperaPromedio->setText("0");
    sprTiempoEsperaPromedio->setVisible(true);
    sprMaxTiempoEspera = new TextSprite(this->disp, "0", 15, salidaDatos.first + 400, 600 - salidaDatos.second - 20);
    sprMaxTiempoEspera->setText("0");
    sprMaxTiempoEspera->setVisible(true);

	this->disp.setTitle("Banco Modelo 4 colas");
	this->disp.setBackground("../images/fondoBlanco.jpg");
}

void Banco::dispActualizarClientesEnSistema()
{
	char* smaxcant = new char[5];
	char* scant = new char[5];
	itoa(this->cantClientesMax, smaxcant, 10);
	sprMaxClientes->setText(smaxcant);
	itoa(this->cantClientesActuales, scant, 10);
	sprClientes->setText(scant);
}

void Banco::dispActualizarLargoMaxColas()
{
	char* smaxcant = new char[5];
	itoa(this->largoFilaMax, smaxcant, 10);
	sprMaxLargo->setText(smaxcant);
}

void Banco::dispActualizarEsperaMax()
{
	char* smaxcant = new char[5];
	itoa(this->tiempoEsperaMax, smaxcant, 10);
	sprMaxTiempoEspera->setText(smaxcant);
}

void Banco::dispActualizarPromedios(unsigned int largoColasP, unsigned int tEsperaPromedio)
{
	char* sLargo = new char[5];
	char* sTiempo = new char[10];
	itoa(largoColasP, sLargo, 10);
	sprLargoPromedio->setText(sLargo);
	itoa(tEsperaPromedio, sTiempo, 10);
	sprTiempoEsperaPromedio->setText(sTiempo);
}



