#include "../headers/streamcypher.h"

STREAMCYPHER::STREAMCYPHER(Options *o)
{
    pSeedArrays = new SEEDARRAYS(o);
    mt1.init_mt(pSeedArrays->getSeed1(), pSeedArrays->getSeedOneFillState());
    mt2.init_mt(pSeedArrays->getSeed2(), pSeedArrays->getSeedTwoFillState());
}

STREAMCYPHER::~STREAMCYPHER()
{
    delete pSeedArrays;
}

void STREAMCYPHER::fillRandomArrays(int length)
{
    mt1.fillArrayWithRandomNumbers(cypherArr1, length);
    mt2.fillArrayWithRandomNumbers(cypherArr2, length);
}

char *STREAMCYPHER::createStreamCypher()
{
    int byte_count = 0;
    unsigned long long mask = 0xff;
    unsigned long long cypherValue32bit;

    fillRandomArrays(block_size);
    
    for(int i = 0; i < block_size; i++)
    {
        cypherValue32bit = cypherArr1[i] ^ cypherArr2[i];
        
        for(int y = 0; y < 4; y++)
        {
            streamCypher[byte_count] = mask & cypherValue32bit;
            cypherValue32bit = cypherValue32bit >> 8;
            byte_count++;
        }
    }

    return streamCypher;
}

//add system generated random array to seed arrays and re-init MT
void STREAMCYPHER::recombineSeedArrays(unsigned long long *pRandomArray)
{
    pSeedArrays->combineSeedArraysWithRandomArray(pRandomArray);
    mt1.init_mt(pSeedArrays->getSeed1(), pSeedArrays->getSeedOneFillState());
    mt2.init_mt(pSeedArrays->getSeed2(), pSeedArrays->getSeedTwoFillState());
}
