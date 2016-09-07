#ifndef RCE_UTILITY_RMATHUTILITY_H
#define RCE_UTILITY_RMATHUTILITY_H


namespace rce {
    namespace utility {

        double generateRandom(double lowBound,
                              double upBound);

        // works on positive values
        long long int roundToOdd(double value);

//        class RMathUtility
//        {
//        public:
//            RMathUtility();
//        };

    } // namespace utility
} // namespace rce

#endif // RCE_UTILITY_RMATHUTILITY_H
