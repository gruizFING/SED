#ifndef STATISTICS_H_INCLUDED
#define STATISTICS_H_INCLUDED

#include "constantes.hpp"

#include <cmath>

double media(double datos[], unsigned int size)
{
    if (size > 1)
    {
        double sum = 0.0;
        for(unsigned int i = 0 ; i < size ; i++ )
        {
            sum += datos[i];
        }
        return (sum/size);
    }
    else
        return -1;
}

double varianza(double datos[], double media, unsigned int size)
{
    if (size > 1)
    {
        double aux = 0.0;
        for(unsigned int i = 0 ; i < size ; i++ )
        {
            aux += pow((datos[i] - media ), 2);
        }
        return (aux/(size-1));
    }
    else
        return -1;
}

double desv_std(double var)
{
    return sqrt(var);
}

double inf_int_confianza(double media, double desv, unsigned int size)
{
    return (media - z * ( desv/sqrt(size)));
}


double sup_int_confianza(double media, double desv, unsigned int size)
{
    return (media + z * ( desv/sqrt(size)));
}

#endif // STATISTICS_H_INCLUDED
