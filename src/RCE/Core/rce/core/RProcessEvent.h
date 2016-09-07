#ifndef RCE_CORE_RPROCESSEVENT_H
#define RCE_CORE_RPROCESSEVENT_H

#include <QString>
#include <QSharedPointer>

#define RCE_CREATE_EVENT_SRC(description, type) rce::core::RProcessEvent::Ptr(new rce::core::RProcessEvent((description), (type), __FILE__, __LINE__))
#define RCE_CREATE_EVENT(description, type) rce::core::RProcessEvent::Ptr(new rce::core::RProcessEvent((description), (type)))

class QColor;

namespace rce {
    namespace core {

        enum RProcessEventType
        {
            RInfoEvent,
            RDebugEvent,
            RWarnEvent,
            RErrorEvent,
            RFatalEvent
        };

        QString getEventTypeString(RProcessEventType eventType);
        QColor getEventTypeColour(RProcessEventType eventType);


        class RProcessEvent
        {
        public:
            typedef QSharedPointer<RProcessEvent> Ptr;
            typedef QSharedPointer<const RProcessEvent> ConstPtr;

        public:
            RProcessEvent(const QString &description,
                          RProcessEventType type = RDebugEvent,
                          const QString &sourceFile = QString(),
                          int lineNumber = 0,
                          quint64 timestamp = 0);


            const QString &getDescription() const {return description_;}
            RProcessEventType getType() const {return type_;}
            const QString &getSourceFile() const  {return file_;}
            int getLineNumber() const {return lineNumber_;}
            quint64 getTimestamp() const {return timeStamp_;}

            QString getReadableString() const;



            bool isSourcePositionDefined() const {return !file_.isEmpty();}
        protected:
            QString description_;
            RProcessEventType type_;
            quint64 timeStamp_;
            QString file_;
            int lineNumber_;
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RPROCESSEVENT_H
