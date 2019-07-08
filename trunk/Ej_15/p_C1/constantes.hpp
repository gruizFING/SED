#ifndef CONSTANTES_HPP_INCLUDED
#define CONSTANTES_HPP_INCLUDED

#include <math.h>

// constantes del modelo
const unsigned int cantCajasM = 4;
const double tiempoEntreArribosP = 33.67347;
const double tiempoServicioNRm = 82.0; // Distribucion para todas las cajas no solo retiro
const double tiempoServicioNRs = 59.63;
const double tiempoServicioSRm = 73.65; // Distribucion para todas las cajas solo retiro
const double tiempoServicioSRs = 60.72;
const unsigned int minutosSimulacionMin = 360;
const double timeIntervalRDTSseg = 30;
const double timeIntervalSwap = 60.0*3.0;
const double maxRateArribos = 213.0/3600.0;

//Analisis de sensibilidad
const double tiempoEntreArribosMin = 3600.0/51.0;  //51 clientes en una hora
const double tiempoEntreArribosMax = 3600.0/213.0; //213 clientes en una hora
const unsigned int cantCajasMin = 3;
const unsigned int cantCajasMax = 4;
const unsigned int minutosSimulacionAS = 60;

//Constante Thining
const double arrayRateArribos [6] = {51.0/3600.0,73.0/3600.0,109.0/3600.0,213.0/3600.0,84.0/3600.0,114.0/3600.0};

#endif // CONSTANTES_HPP_INCLUDED
