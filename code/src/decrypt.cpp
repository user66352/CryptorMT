#include <fstream>

#include "../headers/decrypt.h"

DECRYPT::DECRYPT(Options *o)
{
    infile = o->infile;
    outfile = o->outfile;
    pass = o->pw;
    stdi = o->stdi;
    stdo = o->stdo;
    sc = new STREAMCYPHER(o);
}

DECRYPT::~DECRYPT()
{
    delete sc;
}

int DECRYPT::startDecryption()
{
    const int bufferLength = block_size * 4;      //block_size defined in streamcypher.h
    char ioBuffer[bufferLength];
    unsigned long long seedBuffer[16] = { 0 };

    //opening infile/stdin
    std::istream *is;
    if(stdi)
        is = &std::cin;
    else
        is = new std::ifstream(infile, std::ifstream::binary);

    if(is->fail())
    {
        std::cerr << "Error opening source file: " << infile << std::endl;
        if(!stdi) delete is;
        exit(EXIT_FAILURE);
    }

    //read first 128-byte / encrypted random key
    for (int i = 0; i < 16; i++) is->read((char *)&seedBuffer[i], sizeof(unsigned long long));
    combine((char *)seedBuffer, 16 * sizeof(unsigned long long));

    //expand seedBuffer to 32-bit seed values
    int randomSeedIndex = 0;
    unsigned long long mask = 0x00000000ffffffffULL;
    for (int i = 0; i < 16; i++)
    {
        randomSeedArray[randomSeedIndex] = (mask & seedBuffer[i]);
        randomSeedIndex++;
        randomSeedArray[randomSeedIndex] = (seedBuffer[i] >> 32);
        randomSeedIndex++;
    }

    //combine random bytes with MT seed's
    sc->recombineSeedArrays(randomSeedArray);

    //opening outfile/stdout
    std::ostream *os;
    if(stdo)
        os = &std::cout;
    else
        os = new std::ofstream(outfile, std::ofstream::binary);

    if(os->fail())
    {
        std::cerr << "Error opening target file: " << outfile << std::endl;
        if(!stdo) delete os;
        exit(EXIT_FAILURE);
    }

    //encrypt file with random seed
    int bytesRead;
    while(is->good())
    {
        bytesRead = 0;
        is->read(ioBuffer, bufferLength);
        bytesRead = is->gcount();
        combine(ioBuffer, bytesRead);
        os->write(ioBuffer, bytesRead);
    }

    if(!stdi) delete is;
    if(!stdo) delete os;

    return 0;
}

void DECRYPT::combine(char *buffer, int length)
{
    char *cypher = sc->createStreamCypher();

    for(int i = 0; i < length; i++)
    {
        buffer[i] ^= cypher[i];
    }
}