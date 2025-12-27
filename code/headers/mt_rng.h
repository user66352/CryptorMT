#ifndef MT_RNG_H
#define MT_RNG_H

#include "mt19937ar.h"
#include "seedarrays.h"

class MTRNG
{
    private:

    public:
    void init_mt(unsigned long long *, int );
    unsigned long long * getRandomNumbers(int);                 //not in use, can be deleted?
    void fillArrayWithRandomNumbers(unsigned long long *, int);
};

#endif