#include "../headers/mt_rng.h"

void MTRNG::init_mt(unsigned long long *init_seed, int arr_length)
{
    init_by_array(init_seed, arr_length);
}

unsigned long long *MTRNG::getRandomNumbers(int length)
{
    unsigned long long *arr = new unsigned long long[length];

    for (int i = 0; i < length; i++)
    {
        arr[i] = genrand_int32();
    }

    return arr;
}

void MTRNG::fillArrayWithRandomNumbers(unsigned long long *arr, int length)
{
    for (int i = 0; i < length; i++)
    {
        arr[i] = genrand_int32();
    }
}