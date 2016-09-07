#include "RMathUtility.h"

#include <random>


double
rce::utility::
generateRandom(double lowBound,
               double upBound)
{
    if(upBound < lowBound)
    {
        std::swap(upBound,
                  lowBound);
    }

    if(upBound == lowBound)
    {
        return upBound;
    }


    static std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(lowBound,upBound);
    return distribution(generator);
}

long long int
rce::utility::roundToOdd(double value)
{
    return static_cast<long long int>(value / 2.0) * 2 + 1;
}
