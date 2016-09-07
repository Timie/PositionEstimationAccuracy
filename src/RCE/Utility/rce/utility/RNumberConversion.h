#ifndef RCE_UTILITY_RNUMBERCONVERSION_H
#define RCE_UTILITY_RNUMBERCONVERSION_H

class QString;
class QLocale;

namespace rce {
    namespace utility {


        QString doubleToStr(double number,
                            char f = 'g',
                            int prec = 6);

        QString doubleToStr(const QLocale &l,
                            double number,
                            char f = 'g',
                            int prec = 6);

        double preventNegZero(double value, int prec);

//        class RNumberConversion
//        {
//        public:
//            RNumberConversion();
//        };

    } // namespace utility
} // namespace rce

#endif // RCE_UTILITY_RNUMBERCONVERSION_H
