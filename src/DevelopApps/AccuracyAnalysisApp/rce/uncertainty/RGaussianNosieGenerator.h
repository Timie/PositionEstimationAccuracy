#ifndef RCE_UNCERTAINTY_RGAUSSIANNOSIEGENERATOR_H
#define RCE_UNCERTAINTY_RGAUSSIANNOSIEGENERATOR_H

#include "rce/uncertainty/RUncertaintyGenerator.h"

#include <random>

namespace rce {
    namespace uncertainty {

        class RGaussianNosieGenerator : public RUncertaintyGenerator
        {
            friend class RUncertaintyGenerator;
        protected: // methods
            RGaussianNosieGenerator();


            virtual void loadSettings(QSettings &settings);
            virtual double generateErrValue(double inputValue);

        protected: // attributes


            double sigma_;
            double mu_;
           std::normal_distribution<double> distribution_;
        };

    } // namespace uncertainty
} // namespace rce

#endif // RCE_UNCERTAINTY_RGAUSSIANNOSIEGENERATOR_H
