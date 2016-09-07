#ifndef DFS_CORE_RTIMESTAMP_H
#define DFS_CORE_RTIMESTAMP_H

#include <QtGlobal>

class QDateTime;

namespace dfs {
    namespace core {

        typedef qint64  DTimeStamp; // in miliseconds

        static const qint64 D_INVALID_TIME_STAMP = -1;

        double timeStampDurationToSeconds(const DTimeStamp &ts);
        DTimeStamp secondsToTimeStampDuration(double secs);

        DTimeStamp qDateTimeToTimeStamp(const QDateTime &dateTime);
//        class RTimeStamp
//        {
//        public:
//            RTimeStamp();
//        };

    } // namespace core
} // namespace dfs

#endif // DFS_CORE_RTIMESTAMP_H
