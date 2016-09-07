#ifndef RCE_CORE_RSTDDEVCALCULATOR_H
#define RCE_CORE_RSTDDEVCALCULATOR_H

// uses knuth's algorithm (see https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance, online algorithm)
namespace rce {
    namespace core {

        class RStdDevCalculator
        {
        public:
            RStdDevCalculator();

            void processData(double data);

            void retrieveMeanAndStdDev(double &mean,
                                       double &stdDev) const;

            void retrieveMeanAndVariance(double &mean,
                                       double &variance) const;

            double retrieveMean() const {return mean_;}


            int getNumberOfSamples() const {return n_;}

        protected:
            int n_;
            double mean_;
            double variance_;
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RSTDDEVCALCULATOR_H
