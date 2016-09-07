#include "RGaussianNosieGenerator.h"

#include <QSettings>

rce::uncertainty::RGaussianNosieGenerator::
RGaussianNosieGenerator():
    sigma_(0),
    distribution_(0,1)
{

}

void
rce::uncertainty::RGaussianNosieGenerator::
loadSettings(QSettings &settings)
{
    if(!settings.contains("sigma"))
        settings.setValue("sigma",
                          0.0);
    sigma_ = settings.value("sigma").toDouble();

    if(!settings.contains("mu"))
        settings.setValue("mu",
                          0.0);
    mu_ = settings.value("mu").toDouble();

    double tmpSigma;
    if(sigma_ != 0)
    {
        tmpSigma = sigma_;
    }
    else
    {
        tmpSigma = 1;
    }

    distribution_ = std::normal_distribution<double>(mu_, tmpSigma);
}

double
rce::uncertainty::RGaussianNosieGenerator::
generateErrValue(double inputValue)
{
    if(sigma_ == 0)
    {
        return mu_;
    }
    else
    {
        return distribution_(randomGenerator_);
    }
}
