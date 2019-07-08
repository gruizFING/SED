#ifndef CAJA_HPP_INCLUDED
#define CAJA_HPP_INCLUDED

#include <eosim/core/entity.hpp>

class Caja: public eosim::core::Entity {
public:

	bool soloRetiro; // true si solo atiende clientes por retiro de dinero
    bool disponible;

	Caja(bool retiro) : soloRetiro(retiro), disponible(true) {}
	~Caja() {}
};

#endif // CAJA_HPP_INCLUDED
