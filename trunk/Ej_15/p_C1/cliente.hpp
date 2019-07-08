#ifndef CLIENTE_HPP_INCLUDED
#define CLIENTE_HPP_INCLUDED

#include <eosim/core/entity.hpp>

class Cliente: public eosim::core::Entity {
public:
    unsigned int nro;
	bool retiraDinero;
    int cajaElegida; // -1 si no eligio aun

	Cliente(bool retira) : retiraDinero(retira), cajaElegida(-1) {}
	~Cliente() {}
};

#endif // CLIENTE_HPP_INCLUDED
