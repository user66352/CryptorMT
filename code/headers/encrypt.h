#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <string>

#include "global.h"
#include "SHA512.h"
#include "streamcypher.h"

class ENCRYPT
{
    private:
    std::string pass;
    std::string infile;
    std::string outfile;
    bool stdi = false;
    bool stdo = false;
    unsigned long long randomArray[32] = { 0 };
    STREAMCYPHER *sc;
    SHA512 sha512;

    void fillRandomSeedArray(unsigned long long *arr);

    public:
    ENCRYPT(Options *);
    ~ENCRYPT();
    void combine(char *, int);
    int startEncryption();

};

#endif