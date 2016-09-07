#include "RConstantErrorGenerator.h"

#include <QSettings>

void
rce::uncertainty::RConstantErrorGenerator::
loadSettings(QSettings &settings)
{
    if(!settings.contains("error"))
        settings.setValue("error",
                          0.0);

    error_ = settings.value("error").toDouble();
}

rce::uncertainty::RConstantErrorGenerator::
RConstantErrorGenerator():
    error_(0.0)
{

}

double
rce::uncertainty::RConstantErrorGenerator::
generateErrValue(double inputValue)
{
    Q_UNUSED(inputValue);
    return error_;
}
