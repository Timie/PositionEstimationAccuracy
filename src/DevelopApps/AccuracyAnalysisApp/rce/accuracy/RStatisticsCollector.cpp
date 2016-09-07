#include "RStatisticsCollector.h"

#include <QSettings>

#include "rce/core/RSettingNester.h"


std::shared_ptr<rce::accuracy::RStatisticsCollector>
rce::accuracy::RStatisticsCollector::
createFromSettings(QSettings &settings,
                   const QString &sectionName)
{
    rce::core::RSettingNester n(settings,
                                sectionName);

    if(!settings.contains("histogram_bin_width"))
        settings.setValue("histogram_bin_width",
                          0);

    std::shared_ptr<rce::accuracy::RStatisticsCollector> result = std::make_shared<rce::accuracy::RStatisticsCollector>();

    result->histogramBinWidth_ = settings.value("histogram_bin_width").toDouble();



    // ...
    return result;
}

rce::accuracy::RStatisticsCollector::
RStatisticsCollector():
    expectedValueSet_(false),
    expectedValue_(0),
    avgVariance_(0),
    histogramBinWidth_(-1)
{

}

void
rce::accuracy::
RStatisticsCollector::
setExpectedValue(double e)
{
    expectedValueSet_ = true;
    expectedValue_ = e;
    avgVariance_ = 0;
}

void
rce::accuracy::RStatisticsCollector::
addDatum(double value)
{
    stdDevCollector_.processData(value);
    if(expectedValueSet_)
    {
        double variance = (value - expectedValue_) * (value - expectedValue_);
        double deltaV = variance - avgVariance_;
        avgVariance_ = avgVariance_ + deltaV/stdDevCollector_.getNumberOfSamples();
    }
    if(histogramBinWidth_ > 0)
    {
        double histogramKey = std::round(value / histogramBinWidth_) * histogramBinWidth_;
        histogram_[histogramKey]++;
    }
}

QString
rce::accuracy::RStatisticsCollector::
getReadableReport() const
{
    double mean, stdDev;
    stdDevCollector_.retrieveMeanAndStdDev(mean,
                                           stdDev);
    if(hasExpectedValueSet())
    {
        double devFromExp = getDeviationFromExpectedValue();

        return QString("n=%1; mu=%2; sigma=%3; devFromExp=%4;")
                .arg(stdDevCollector_.getNumberOfSamples())
                .arg(mean)
                .arg(stdDev)
                .arg(devFromExp);
    }
    else
    {
        return QString("n=%1; mu=%2; sigma=%3;")
                .arg(stdDevCollector_.getNumberOfSamples())
                .arg(mean)
                .arg(stdDev);
    }
}

QString
rce::accuracy::RStatisticsCollector::
getHistogramReportCSV() const
{
    if(histogramBinWidth_ > 0)
    {
        QString result;
        for(auto it = histogram_.begin();
            it != histogram_.end();
            ++it)
        {
            result.append(QString("%1;%2;\n")
                          .arg(it->first)
                          .arg(it->second));
        }

        return result;
    }
    else
    {
        return "No histogram data collected.";
    }
}

double
rce::accuracy::RStatisticsCollector::
getSigma() const
{

    double mean, stdDev;
    stdDevCollector_.retrieveMeanAndStdDev(mean,
                                           stdDev);

    return stdDev;
}

double
rce::accuracy::RStatisticsCollector::
getMean() const
{
    return stdDevCollector_.retrieveMean();
}

double
rce::accuracy::RStatisticsCollector::
getDeviationFromExpectedValue() const
{
    return std::sqrt(avgVariance_);
}

void rce::accuracy::RStatisticsCollector::clearData()
{
    stdDevCollector_ = rce::core::RStdDevCalculator();
    avgVariance_ = 0;
    histogram_.clear();
}
