#ifndef COMPRADOR_H
#define COMPRADOR_H

#include <eosim\core\entity.hpp>
#include "paciente.hpp"

namespace eosim {

namespace core {

    class Comparador: public EntityComp {
        public:
            // constructor
            Comparador();
            // funcion virtual compare
            bool compare (Entity* p1, Entity* p2);
            // destructor
            ~Comparador();
    };
}

}

#endif // COMPRADOR_H
