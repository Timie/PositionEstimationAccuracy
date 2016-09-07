#ifndef RCE_CORE_RDATETIME_H
#define RCE_CORE_RDATETIME_H

#include <QString>

namespace rce {
    namespace core {

        class RDateTime
        {
        public:
//            RDateTime();
//            ~RDateTime();

            // works only for positive seconds
            static QString getTimeDurationString(double seconds,
                                                 int secPrecision = 2,
                                                 const QString &hourStr = QString(),
                                                 const QString &minuteStr = QString(),
                                                 const QString &secondStr = QString(),
                                                 bool forceShowHour = false,
                                                 bool forceShowMinute = false,
                                                 bool forceShowSeconds = false,
                                                 int minHourDigits = 0,
                                                 int minMinuteDigits = 0,
                                                 int minSecondDigits = 0,
                                                 bool showSucceeding = false,
                                                 bool expandNextDigits = false);
            static void splitSeconds(double secondsIn,
                                     int secPrecision,
                                     double *miliseconds,
                                     int *seconds,
                                     int *minutes,
                                     int *hours); // pointers can be set to NULL - the respective entry will be skipped
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RDATETIME_H
