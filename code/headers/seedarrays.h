#ifndef SEEDARRAYS_H
#define SEEDARRAYS_H

#include <iostream>
#include "global.h"

#define seedLength 624

class SEEDARRAYS
{
    private:
    unsigned long long seed1[seedLength] = { 0 };
    unsigned long long seed2[seedLength] = { 0 };
    int seedArrayPointer1 = 0;                      //pointer to the next free position in seed array1(2)
    int seedArrayPointer2 = 0;
    int keyfileBufferSize = 0;
    char keyfileBuffer[keyFileMaxBytes] = { 0 };

    int readKeyfile(std::string);
    void combineKeyfileWithSeedArrays();
    void addKeyHashToSeed(unsigned long long *, int);
    int addArrayToSeed(unsigned long long *, int, int);
    void combineSeedArrayWithBuffer(unsigned long long *, unsigned char *, int, int);
    void combineKeyWithSeedArrays(std::string);

    //DEBUG SECTION
    void printSeedArray(unsigned long long *);
    void printHassArray(unsigned long long *);
    
    public:
    SEEDARRAYS(Options *);
    void combineSeedArraysWithRandomArray(unsigned long long *);
    int getSeedOneFillState();
    int getSeedTwoFillState();
    unsigned long long * getSeed1();
    unsigned long long * getSeed2();
};

#endif