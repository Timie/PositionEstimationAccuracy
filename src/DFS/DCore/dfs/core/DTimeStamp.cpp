#include "DTimeStamp.h"

#include <QDateTime>


double
dfs::core::
timeStampDurationToSeconds(const dfs::core::DTimeStamp &ts)
{
    return ts / 1000.0;
}


dfs::core::DTimeStamp
dfs::core::
secondsToTimeStampDuration(double secs)
{
    return qRound64(secs * 1000);
}


dfs::core::DTimeStamp
dfs::core::
qDateTimeToTimeStamp(const QDateTime &dateTime)
{
    return dateTime.toMSecsSinceEpoch();
}
