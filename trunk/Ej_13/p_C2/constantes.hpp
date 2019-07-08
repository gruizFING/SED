#ifndef CONSTANTES_HPP_INCLUDED
#define CONSTANTES_HPP_INCLUDED

#include <math.h>

// constantes del modelo
const unsigned int cantCajasM = 4;
const double tiempoEntreArribosP = 33.67347;
const double tiempoServicioM = 78.39; // Distribucion para todas las cajas
const double tiempoServicioS = 57.51;
const double tiempoServicioNRm = 82.0; // Distribucion para todas las cajas no solo retiro
const double tiempoServicioNRs = 59.63;
const double tiempoServicioSRm = 73.65; // Distribucion para todas las cajas solo retiro
const double tiempoServicioSRs = 60.72;
const unsigned int minutosSimulacionMin = 480;
const double timeIntervalRDTSseg = 30;

//Analisis de sensibilidad
const double tiempoEntreArribosMin = 3600/51;  //51 clientes en una hora
const double tiempoEntreArribosMax = 3600/213; //213 clientes en una hora
const unsigned int cantCajasMin = 3;
const unsigned int cantCajasMax = 4;
const unsigned int minutosSimulacionAS = 60;

#endif // CONSTANTES_HPP_INCLUDED
