#ifndef CAJA_HPP_INCLUDED
#define CAJA_HPP_INCLUDED

#include <eosim/core/entity.hpp>
#include <eosim/graphic.hpp>
#include <iostream>


class Caja: public eosim::core::Entity {
public:
	bool soloRetiro; // true si solo atiende clientes por retiro de dinero
    bool disponible;

    std::pair<double,double> pos;
	eosim::graphic::Sprite spr;
	eosim::graphic::TextSprite textSpr;

	Caja(eosim::graphic::Display& disp, std::pair<double,double> position, bool retiro) :
                    soloRetiro(retiro),
                    disponible(true),
                    pos(position)
                    {
                        if (retiro) {
                            spr = eosim::graphic::Sprite(disp, "../images/atiende2.jpg", pos.first, pos.second);
                            textSpr = eosim::graphic::TextSprite(disp, "Retiro", 14, pos.first + 80, pos.second + 20);
                        }
                        else
                            spr = eosim::graphic::Sprite(disp, "../images/atiende1.jpg", pos.first, pos.second);

                    }
	~Caja() {}
};

#endif // CAJA_HPP_INCLUDED
