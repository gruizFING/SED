#ifndef COMPRADOR_H
#define COMPRADOR_H


namespace eosim {

namespace core {

    class Comparador: public EntityComp {
        private:
            Entity* e1;
            Entity* e2;

        public:
            // constructor
            Comprador(){};
            // funcion virtual compare
            bool compare (Entity* e1, Entity* e2);
            // destructor
            ~Comparador(){};

    };
}

}

#endif // COMPRADOR_H
