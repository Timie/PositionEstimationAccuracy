#include "RStdDevCalculator.h"

#include <limits>
#include <cmath>




rce::core::RStdDevCalculator::
RStdDevCalculator():
    n_(0),
    mean_(0),
    variance_(0)
{
}

void
rce::core::RStdDevCalculator::
processData(double data)
{
    ++n_;
    double delta = data - mean_;
    mean_ = mean_ + delta/n_;
    variance_ = variance_ + delta * (data - mean_);
}

void
rce::core::RStdDevCalculator::
retrieveMeanAndStdDev(double &mean,
                      double &stdDev) const
{
    double variance;
    retrieveMeanAndVariance(mean,
                            variance);
    stdDev = std::sqrt(variance);
}

void
rce::core::RStdDevCalculator::
retrieveMeanAndVariance(double &mean,
                        double &variance) const
{
    mean = mean_;
    if(n_ > 0)
    {
        variance = variance_ / (n_); // QUESTION: maybe use n - 1 for sample deviation
    }
    else
    {
        variance = std::numeric_limits<double>::quiet_NaN();
    }
}
