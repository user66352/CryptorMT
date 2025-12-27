#ifndef STREAMCYPHER_H
#define STREAMCYPHER_H

#include "mt_rng.h"
#include "global.h"
#include "seedarrays.h"

const int block_size = 2560;            //amount of random 32bit values created at once, must be a multiple of 4 (2560 * 4 = 10k random characters)

class STREAMCYPHER
{
    private:
    MTRNG mt1;
    MTRNG mt2;
    SEEDARRAYS *pSeedArrays;
    unsigned long long cypherArr1[block_size];
    unsigned long long cypherArr2[block_size];
    char streamCypher[block_size * 4];

    void fillRandomArrays(int);

    public:
    STREAMCYPHER(Options *);
    ~STREAMCYPHER();
    char *createStreamCypher();
    void recombineSeedArrays(unsigned long long *);
};

#endif