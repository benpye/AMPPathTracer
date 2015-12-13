#pragma once

#include "Vector3.h"

// glibc values
#define LCG_RANDOM_A 69069
#define LCG_RANDOM_C 1

#define LCG_RANDOM_MAX 4294967295

class LCGRandom
{
public:
    LCGRandom() restrict(amp, cpu)
    {
        x = 0;
    }

    LCGRandom(unsigned int seed) restrict(amp, cpu)
    {
        x = seed;
    }

    unsigned int Next() restrict(amp, cpu)
    {
        x = x * LCG_RANDOM_A + LCG_RANDOM_C;
        return x;
    }

    float NextFloat() restrict(amp, cpu)
    {
        x = x * LCG_RANDOM_A + LCG_RANDOM_C;
        return (float)Next() / (float)LCG_RANDOM_MAX;
    }

    V3 NextVectorInCone(const V3& dir, float angle)
    {

    }
private:
    unsigned int x;
};