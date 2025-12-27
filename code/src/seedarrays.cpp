#include <fstream>
#include <filesystem>

#include <iostream>
#include <iomanip>

#include "../headers/SHA512.h"
#include "../headers/seedarrays.h"

SEEDARRAYS::SEEDARRAYS(Options *o)
{
    // if password exist -> combine it with seed-arrays
    if(!o->pw.empty()) combineKeyWithSeedArrays(o->pw);

    // if keyfile exist -> read keyfile and combine it with seed-arrays
    if(!o->keyfile.empty()) 
    {
        readKeyfile(o->keyfile);
        combineKeyfileWithSeedArrays();
    }
}

void SEEDARRAYS::combineSeedArraysWithRandomArray(unsigned long long *pRandomArray)
{
    int ull_count = 16;
    unsigned long long *pRandomArrayPartOne = &pRandomArray[0];
    unsigned long long *pRandomArrayPartTwo = &pRandomArray[16];

    //seed1
    for (int i = 0; i < ull_count; i++) seed1[i] ^= pRandomArrayPartOne[i];

    //seed2
    for (int i = 0; i < ull_count; i++) seed2[i] ^= pRandomArrayPartTwo[i];
}

void SEEDARRAYS::combineKeyWithSeedArrays(std::string key)
{
    SHA512 sha512;
    HashResult pwHash1, pwHash2;
    
    sha512.hash512(key, &pwHash1);
    sha512.hash512(pwHash1.digestString, &pwHash2);

    addKeyHashToSeed(pwHash1.hashBinary, 1);
    addKeyHashToSeed(pwHash2.hashBinary, 2);
}

void SEEDARRAYS::addKeyHashToSeed(unsigned long long *binaryKeyHash, int seedIndex)
{
	int tempArrIndex = 0;
    int tempArrLength = 16;
	unsigned long long tempArr[16] = { 0 };
	unsigned long long mask = 0x00000000ffffffff;
	
	for (int i = 0; i < 8; i++)
	{
		tempArr[tempArrIndex] = (binaryKeyHash[i] & mask);
		binaryKeyHash[i] = binaryKeyHash[i] >> 32;
		tempArrIndex++;
		tempArr[tempArrIndex] = binaryKeyHash[i];
		tempArrIndex++;
	}
	
	addArrayToSeed(tempArr, seedIndex, tempArrLength);
}

//arr needs to have all relevant bits in the 32 lower bits per value (0x00000000ef091abc)
//seedIndex is pointing to either seed1 or seed2 (1 = seed1; 2 = seed2)
//returns the amount of ULL values added from array to seed
int SEEDARRAYS::addArrayToSeed(unsigned long long * arr, int seedIndex, int length)
{
    int *pNextFreeSeedIndex;
    unsigned long long *pSeed = nullptr;

    if (seedIndex == 1)
    {
        pSeed = &seed1[seedArrayPointer1];
        pNextFreeSeedIndex = &seedArrayPointer1;
    } 
    if (seedIndex == 2)
    {
        pSeed = &seed2[seedArrayPointer2];
        pNextFreeSeedIndex = &seedArrayPointer2;
    }

    if (pSeed == nullptr)
    {
        std::cerr << "Error in selecting a seed array!\n";
        exit(EXIT_FAILURE);
    }

    int available = seedLength - (*pNextFreeSeedIndex);
    available = std::max(available, 0);
    int possible = std::min(available, length);
    for (int i = 0; i < possible; i++) pSeed[i] = arr[i];
    *pNextFreeSeedIndex = (*pNextFreeSeedIndex) + possible;

    return std::min(possible, length);
}

void SEEDARRAYS::combineKeyfileWithSeedArrays()
{
    int bytesPerArray;
    unsigned char *bytePointer = (unsigned char *) keyfileBuffer;

    bytesPerArray = keyfileBufferSize / 2;

    combineSeedArrayWithBuffer(&seed1[seedArrayPointer1], &bytePointer[0], bytesPerArray, 1);
    combineSeedArrayWithBuffer(&seed2[seedArrayPointer2], &bytePointer[bytesPerArray], bytesPerArray, 2);
}

void SEEDARRAYS::combineSeedArrayWithBuffer(unsigned long long *seed, unsigned char *buffer, int bufferLength, int seedArrIndex)
{
    int ullIndex = 0;
    int seedIndex = 0;
    int *pSeedArrayPointer;
    int availableFreeBytes;

    if (seedArrIndex == 1)
    {
        availableFreeBytes = (seedLength - (seedArrayPointer1)) * 4;        //4 bytes used per ULL in seed array
        pSeedArrayPointer = &seedArrayPointer1;
    }
    if (seedArrIndex == 2)
    {
        availableFreeBytes = (seedLength - (seedArrayPointer2)) * 4;
        pSeedArrayPointer = &seedArrayPointer2;
    }

    int cycles = std::min(availableFreeBytes, bufferLength);
    for(int i = 0; i < cycles; i++)
    {
        seed[seedIndex] = seed[seedIndex] << 8;
        seed[seedIndex] = seed[seedIndex] | buffer[i];
        ullIndex = (ullIndex + 1) % 4;
        if(!ullIndex) seedIndex++;
    }

    *pSeedArrayPointer = (*pSeedArrayPointer) + seedIndex + 1;      //point at the first free position in seed array
}

int SEEDARRAYS::readKeyfile(std::string keyfileName)
{
    int kfSize = 0;
    int bytesToRead = 0;
    std::ifstream kf;

    kf.open(keyfileName, std::ifstream::binary);

    if(kf.fail())
    {
        std::cerr << "Error opening keyfile: " << keyfileName << std::endl;
        exit(EXIT_FAILURE);
    }

    kfSize = std::filesystem::file_size(keyfileName);

    if(kfSize < keyFileLengthMin)
    {
        std::cerr << "Keyfile size needs to be at least " << keyFileLengthMin << " bytes." << std::endl;
        kf.close();
        exit(EXIT_FAILURE);
    }

    //make sure to read an even amount from keyfile
    bytesToRead = kfSize - (kfSize % 2);
    bytesToRead = std::min(bytesToRead, keyFileMaxBytes);

    kf.read(keyfileBuffer, bytesToRead);

    if(kf.gcount() != bytesToRead)
    {
        std::cerr << "Error reading required bytes from keyfile: " << keyfileName << std::endl;
        kf.close();
        exit(EXIT_FAILURE);
    }

    kf.close();
    keyfileBufferSize = bytesToRead;
    
    return 0;
}

int SEEDARRAYS::getSeedOneFillState()
{
    return seedArrayPointer1;
}

int SEEDARRAYS::getSeedTwoFillState()
{
    return seedArrayPointer2;
}

unsigned long long * SEEDARRAYS::getSeed1()
{
    return seed1;
}

unsigned long long * SEEDARRAYS::getSeed2()
{
    return seed2;
}