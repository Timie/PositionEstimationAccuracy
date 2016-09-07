#ifndef RCE_UNCERTAINTY_RCONSTANTERRORGENERATOR_H
#define RCE_UNCERTAINTY_RCONSTANTERRORGENERATOR_H

#include "rce/uncertainty/RUncertaintyGenerator.h"

namespace rce {
    namespace uncertainty {

        class RConstantErrorGenerator : public RUncertaintyGenerator
        {
            friend class RUncertaintyGenerator;

        protected: // methods
            RConstantErrorGenerator();

            virtual void loadSettings(QSettings &settings);
            virtual double generateErrValue(double inputValue);

        protected: // attributes

            double error_;
        };

    } // namespace uncertainty
} // namespace rce

#endif // RCE_UNCERTAINTY_RCONSTANTERRORGENERATOR_H
