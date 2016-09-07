#include "RUncertaintyGenerator.h"

#include <ctime>

#include <QSettings>

#include "rce/core/RSettingNester.h"
#include "rce/uncertainty/RConstantErrorGenerator.h"
#include "rce/uncertainty/RGaussianNosieGenerator.h"


std::shared_ptr<rce::uncertainty::RUncertaintyGenerator>
rce::uncertainty::RUncertaintyGenerator::
createFromSettings(QSettings &settings,
                          const QString &sectionName)
{
    rce::core::RSettingNester n(settings,
                                sectionName);


    if(!settings.contains("generator_type"))
        settings.setValue("generator_type",
                          typeToString(RConstantErrorType));

    std::shared_ptr<rce::uncertainty::RUncertaintyGenerator> generator;

    switch(stringToType(settings.value("generator_type").toString()))
    {
        case rce::uncertainty::RUncertaintyGenerator::RConstantErrorType:
            generator = std::shared_ptr<RConstantErrorGenerator>(new RConstantErrorGenerator());
            break;
        case rce::uncertainty::RUncertaintyGenerator::RGaussianNoiseType:
            generator = std::shared_ptr<RGaussianNosieGenerator>(new RGaussianNosieGenerator());
            break;

    }



    if(!settings.contains("relative"))
        settings.setValue("relative",
                          "false");
    generator->relative_  = (settings.value("relative").toString().toLower() == "true");

    if(!settings.contains("random_seed"))
        settings.setValue("random_seed",
                          -1);
    long long int seedValue = settings.value("random_seed").toLongLong();
    if(seedValue > 0)
    {
        generator->seed(seedValue);
    }
    else if(seedValue < 0)
    {
        static std::seed_seq seq{time(NULL)};
        std::vector<quint32> seeds(1);
        seq.generate(seeds.begin(),
                     seeds.end());
        generator->seed(seeds[0]);
    }

    generator->loadSettings(settings);


    if(!settings.contains("has_child_generator"))
        settings.setValue("has_child_generator",
                          "false");

    if(settings.value("has_child_generator").toString().toLower() == "true")
    {
        generator->childGenerator_ = createFromSettings(settings,
                                                               "child_generator");
    }

    return generator;
}

QString
rce::uncertainty::RUncertaintyGenerator::
typeToString(rce::uncertainty::RUncertaintyGenerator::RGeneratorType type)
{
    switch(type)
    {
        case rce::uncertainty::RUncertaintyGenerator::RConstantErrorType:
            return "constant_error";
            break;
        case rce::uncertainty::RUncertaintyGenerator::RGaussianNoiseType:
            return "gaussian_noise";
            break;
        default:
            {
                throw "RUncertaintyGenerator::typeToString: Invalid type.";
            }

            return "constant_error";
            break;

    }
}

rce::uncertainty::RUncertaintyGenerator::RGeneratorType
rce::uncertainty::RUncertaintyGenerator::
stringToType(const QString &typeStr)
{
    QString typeStrLow = typeStr.toLower();
    if(typeStrLow == "constant_error")
    {
        return RConstantErrorType;
    }
    else if(typeStrLow == "gaussian_noise")
    {
        return RGaussianNoiseType;
    }
    else
    {
        throw "RUncertaintyGenerator::stringToType: Unkown type.";
        return RConstantErrorType;
    }
}


double
rce::uncertainty::RUncertaintyGenerator::
generateValue(double inputValue)
{
    double err = generateErrValue(inputValue);
    if(relative_)
    {
        err = err / std::abs(inputValue);
    }

    if(childGenerator_)
    {
        return childGenerator_->generateValue(err + inputValue);
    }
    else
    {
        return err + inputValue;
    }
}

void rce::uncertainty::
RUncertaintyGenerator::
seed(unsigned long seedVal)
{
    randomGenerator_.seed(seedVal);
}

rce::uncertainty::RUncertaintyGenerator::
RUncertaintyGenerator():
    relative_(false)
{

}
