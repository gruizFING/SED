#ifndef CLIENTE_HPP_INCLUDED
#define CLIENTE_HPP_INCLUDED

#include <eosim/core/entity.hpp>
#include <eosim/graphic.hpp>

#include <eosim/core/entity.hpp>
#include <eosim/graphic.hpp>

class Cliente: public eosim::core::Entity {
public:
    unsigned int nro;
	bool retiraDinero;
    int cajaElegida; // -1 si no eligio aun

    std::pair<double,double> pos;
	eosim::graphic::Sprite spr;

	Cliente(eosim::graphic::Display& disp, std::pair<double,double> position, bool retira) :
	    retiraDinero(retira),
        cajaElegida(-1),
        pos(position)
        {
            if (retira)
                spr = eosim::graphic::Sprite(disp, "../images/client1.jpg", pos.first, pos.second);
            else
                spr = eosim::graphic::Sprite(disp, "../images/client2.jpg", pos.first, pos.second);
        }

	~Cliente() {}
};

#endif // CLIENTE_HPP_INCLUDED
