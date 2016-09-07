#ifndef RCE_UNCERTAINTY_RUNCERTAINTYGENERATOR_H
#define RCE_UNCERTAINTY_RUNCERTAINTYGENERATOR_H

#include <memory>
#include <random>

#include <QString>

class QSettings;

namespace rce {
    namespace uncertainty {

        /**
         * @brief The RUncertaintyGenerator class - base class for all uncertainty generators
         */
        class RUncertaintyGenerator
        {
        public:
            enum RGeneratorType
            {
                RConstantErrorType,
                RGaussianNoiseType
            };


            static std::shared_ptr<RUncertaintyGenerator> createFromSettings(QSettings &settings,
                                                                             const QString &sectionName = QString());
            static QString typeToString(RGeneratorType type);
            static RGeneratorType stringToType(const QString &typeStr);

            double generateValue(double inputValue);

            void seed(unsigned long seedVal);

        protected:
            virtual void loadSettings(QSettings &settings) = 0;
            virtual double generateErrValue(double inputValue) = 0;

            RUncertaintyGenerator();


            bool relative_;
            std::shared_ptr<RUncertaintyGenerator> childGenerator_;
            std::default_random_engine randomGenerator_;

        };

    } // namespace uncertainty
} // namespace rce

#endif // RCE_UNCERTAINTY_RUNCERTAINTYGENERATOR_H
