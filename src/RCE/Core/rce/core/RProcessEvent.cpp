#include "RProcessEvent.h"

#include <QDateTime>
#include <QObject>
#include <QColor>

rce::core::RProcessEvent::
RProcessEvent(const QString &description,
              rce::core::RProcessEventType type,
              const QString &sourceFile,
              int lineNumber,
              quint64 timestamp):
    description_(description),
    type_(type),
    timeStamp_(timestamp),
    file_(sourceFile),
    lineNumber_(lineNumber)
{
    if(timeStamp_ == 0)
    {
        timeStamp_ = QDateTime::currentMSecsSinceEpoch();
    }
}

QString
rce::core::RProcessEvent::
getReadableString() const
{
    QDateTime date = QDateTime::fromMSecsSinceEpoch(timeStamp_);
    QString readableString = QString("[") +date.toString() + "] " + getEventTypeString(type_) + ": " + description_;
    if(isSourcePositionDefined())
    {
        readableString += " (" + file_ + ":" + QString::number(lineNumber_) + ").";
    }
    else
    {
        readableString += ".";
    }

    return readableString;
}


QString
rce::core::
getEventTypeString(rce::core::RProcessEventType eventType)
{
   switch(eventType)
   {
       case RInfoEvent:
           return QObject::tr("INFO");
       case RDebugEvent:
           return QObject::tr("DEBUG");
       case RWarnEvent:
           return QObject::tr("WARNING");
       case RErrorEvent:
           return QObject::tr("ERROR");
       case RFatalEvent:
           return QObject::tr("FATAL");
       default:
           return QObject::tr("UNKNOWN");
   }
}


QColor
rce::core::
getEventTypeColour(rce::core::RProcessEventType eventType)
{
    switch(eventType)
    {
        case RInfoEvent:
            return QColor(0,0,0);
        case RDebugEvent:
            return QColor(0,0,255);
        case RWarnEvent:
            return QColor(0,0,0);
        case RErrorEvent:
            return QColor(255,127,0);
        case RFatalEvent:
            return QColor(255,0,0);
        default:
            return QColor(0,0,0);
    }
}
