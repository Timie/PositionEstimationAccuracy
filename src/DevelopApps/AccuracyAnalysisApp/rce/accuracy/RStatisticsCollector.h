#ifndef RCE_ACCURACY_RSTATISTICSCOLLECTOR_H
#define RCE_ACCURACY_RSTATISTICSCOLLECTOR_H

#include <memory>
#include <map>

#include <QString>

#include "rce/core/RStdDevCalculator.h"


class QSettings;

namespace rce {
    namespace accuracy {


        class RStatisticsCollector
        {
        public:

            static std::shared_ptr<RStatisticsCollector> createFromSettings(QSettings &settings,
                                                                      const QString &sectionName = QString());


            RStatisticsCollector();


            void setExpectedValue(double e);

            void addDatum(double value);

            QString getReadableReport() const;
            QString getHistogramReport() const; // human readable
            QString getHistogramReportCSV() const; // csv

            int getDataCount() const {return stdDevCollector_.getNumberOfSamples();}
            double getSigma() const;
            double getMean() const;
            bool hasExpectedValueSet() const {return expectedValueSet_;}
            double getDeviationFromExpectedValue() const;
            double getExpectedValue() const {return expectedValue_;}

            void clearData();

        protected:
            // for now, only std dev and mean...
            rce::core::RStdDevCalculator stdDevCollector_;

            bool expectedValueSet_;
            double expectedValue_;
            double avgVariance_;

            double histogramBinWidth_;
            std::map<double,long long int> histogram_;
        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_RSTATISTICSCOLLECTOR_H
