#include <random>
#include <fstream>

#include "../headers/encrypt.h"

ENCRYPT::ENCRYPT(Options *o)
{
    infile = o->infile;
    outfile = o->outfile;
    pass = o->pw;
    stdi = o->stdi;
    stdo = o->stdo;
    sc = new STREAMCYPHER(o);
}

ENCRYPT::~ENCRYPT()
{
    delete sc;
}

int ENCRYPT::startEncryption()
{
    const int bufferLength = block_size * 4;                    //block_size defined in streamcypher.h
    char ioBuffer[bufferLength];

    fillRandomSeedArray(randomArray);                           //generate system random data for encryption

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

    //write randomArray to outfile
    //compress randomArray into outBuffer for writing
    unsigned long long outBuffer[16] = { 0 };
    int outBufferCounter = 0;

    for (int i = 0; i < 32; i++)
    {
        outBuffer[outBufferCounter] = randomArray[i];
        i++;
        outBuffer[outBufferCounter] = outBuffer[outBufferCounter] | (randomArray[i] << 32);
        outBufferCounter++;
    }

    //encrypt random keys before writing to disk
    combine((char *)outBuffer, 16 * sizeof(unsigned long long));
    for (int i = 0; i < 16; i++) os->write((char *)&outBuffer[i], sizeof(unsigned long long));

    //combine random bytes with MT seed's
    sc->recombineSeedArrays(randomArray);

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

void ENCRYPT::combine(char * buffer, int length)
{
    char *cypher = sc->createStreamCypher();

    for(int i = 0; i < length; i++)
    {
        buffer[i] ^= cypher[i];
    }
}

void ENCRYPT::fillRandomSeedArray(unsigned long long *arr)
{
    int ull_count = 32;

    static std::uniform_int_distribution<unsigned long long> d(0x00ULL, 0xffffffffULL);
    std::random_device&& rd = std::random_device{};             //system dependent; Default token for random_device is usually /dev/urandom on Linux
    for (int i = 0; i < ull_count; i++) arr[i] =  d(rd);        //get 2x512 random bits (32 x 32bit ULL -> 128 byte) from system
}
