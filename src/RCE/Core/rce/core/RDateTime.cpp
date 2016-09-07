#include "RDateTime.h"

#include <QObject>
#include <QStringList>

#include "rce/utility/RNumberConversion.h"

QString
rce::core::RDateTime::
getTimeDurationString(double seconds,
                      int secPrecision,
                      const QString &hourStr,
                      const QString &minuteStr,
                      const QString &secondStr,
                      bool forceShowHour,
                      bool forceShowMinute,
                      bool forceShowSeconds,
                      int minHourDigits,
                      int minMinuteDigits,
                      int minSecondDigits,
                      bool showSucceeding, bool expandNextDigits)
{
    QString hourString = QObject::tr(" h ");
    QString minuteString = QObject::tr(" m ");
    QString secondString = QObject::tr(" s ");

    if(!hourStr.isNull())
        hourString = hourStr;
    if(!minuteStr.isNull())
        minuteString = minuteStr;
    if(!secondStr.isNull())
        secondString = secondStr;

    double milisecs;
    int secondsSplit, minutesSplit, hoursSplit;
    splitSeconds(seconds,
                 secPrecision,
                 &milisecs,
                 &secondsSplit,
                 &minutesSplit,
                 &hoursSplit);

    seconds = secondsSplit + milisecs / 1000.0;

    QStringList entries;
    if((hoursSplit > 0) || forceShowHour)
    {
        // create string
        QString hoursString = QString("%1%2")
                          .arg(hoursSplit)
                          .arg(hourString);
        // prepend zeros if necessary
        int hourDigits = 1;
        while(hoursSplit >= 10)
        {
            hourDigits++;
            hoursSplit = hoursSplit / 10;
        }

        for(int i = hourDigits;
            i < minHourDigits;
            ++i)
        {
            hoursString.prepend("0");
        }

        // set the string
        entries.push_back(hoursString);

        // propagate settings
        if(expandNextDigits)
        {
            minMinuteDigits = qMax(2, minMinuteDigits);
        }

        if(showSucceeding)
        {
            forceShowMinute = true;
        }

    }
    if((minutesSplit > 0) ||
       forceShowMinute)
    {
        // create string
        QString minutesString = QString("%1%2")
                          .arg(minutesSplit)
                          .arg(minuteString);
        // prepend zeros if necessary
        int minuteDigits = 1;
        while(minutesSplit >= 10)
        {
            minuteDigits++;
            minutesSplit = minutesSplit / 10;
        }

        for(int i = minuteDigits;
            i < minMinuteDigits;
            ++i)
        {
            minutesString.prepend("0");
        }

        // set the string
        entries.push_back(minutesString);

        // propagate settings
        if(expandNextDigits)
        {
            minSecondDigits = qMax(2, minSecondDigits);
        }

        if(showSucceeding)
        {
            forceShowSeconds = true;
        }

        // ************************************
    }

    if(entries.empty() ||
       (seconds > 0) ||
       forceShowSeconds)
    {
        QString secondsString = QString("%1%2")
                                .arg(rce::utility::doubleToStr(seconds,
                                                     'f',
                                                     secPrecision))
                                .arg(secondString);
        int secondDigits = 1;
        while(seconds >= 10)
        {
            secondDigits++;
            seconds = seconds / 10;
        }
        for(int i = secondDigits;
            i < minSecondDigits;
            ++i)
        {
            secondsString.prepend("0");
        }

        entries.push_back(secondsString);
    }

    return entries.join("").trimmed();
}

void
rce::core::RDateTime::
splitSeconds(double secondsIn,
             int secPrecision,
             double *miliseconds,
             int *seconds,
             int *minutes,
             int *hours)
{
    double secPrecisionCoefficient = 1;
    for(int i = 0;
        i < secPrecision;
        ++i)
    {
        secPrecisionCoefficient *= 10;
    }
    double secsRounded = qRound(secondsIn * secPrecisionCoefficient) / secPrecisionCoefficient;


    if(hours != NULL)
    {
        // see, how many hours we got
        (*hours) = qRound(floor(secsRounded)) / 3600;
        secsRounded = secsRounded - (*hours) * 3600;
    }

    if(minutes != NULL)
    {
        // see how many minutes we got
        (*minutes) = qRound(floor(secsRounded)) / 60;
        secsRounded = secsRounded - (*minutes) * 60;
    }

    if(seconds != NULL)
    {
        // see how many seconds we got
        (*seconds) = qRound(floor(secsRounded)) / 60;
        secsRounded = secsRounded - (*seconds) * 60;
    }

    if(miliseconds != NULL)
    {
        //the rest is just miliseconds
        (*miliseconds) = secsRounded * 1000.0;
    }
}
