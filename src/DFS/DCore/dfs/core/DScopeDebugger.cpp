#include "DScopeDebugger.h"

#include "dfs/core/DDebug.h"

#ifdef QT_MESSAGELOG_FILE
#undef QT_MESSAGELOG_FILE
#endif
#ifdef QT_MESSAGELOG_LINE
#undef QT_MESSAGELOG_LINE
#endif
#ifdef QT_MESSAGELOG_FUNC
#undef QT_MESSAGELOG_FUNC
#endif

#define QT_MESSAGELOG_FILE this->file_.toLocal8Bit().constData()
#define QT_MESSAGELOG_LINE this->line_
#define QT_MESSAGELOG_FUNC this->function_.toLocal8Bit().constData()

dfs::core::DScopeDebugger::
DScopeDebugger(dfs::core::DScopeDebugger::DStreamType type,
               const char *message,
               const char *file,
               int line,
               const char *function):
    outType_(type),
    message_(message),
    file_(file),
    line_(line),
    function_(function)
{
    if(message_.isEmpty())
    {
        switch(outType_)
        {
            case dfs::core::DScopeDebugger::DInfoStreamType:
                dInfo() << "IN";
                break;
            case dfs::core::DScopeDebugger::DDebugStreamType:
                dDebug() << "IN";
                break;
            case dfs::core::DScopeDebugger::DWarningStreamType:
                dWarning() << "IN";
                break;

        }
    }
    else
    {
        switch(outType_)
        {
            case dfs::core::DScopeDebugger::DInfoStreamType:
                dInfo() << "IN:" << message_;
                break;
            case dfs::core::DScopeDebugger::DDebugStreamType:
                dDebug() << "IN:" << message_;
                break;
            case dfs::core::DScopeDebugger::DWarningStreamType:
                dWarning() << "IN:" << message_;
                break;

        }
    }
}

dfs::core::DScopeDebugger::~DScopeDebugger()
{
    if(message_.isEmpty())
    {
        switch(outType_)
        {
            case dfs::core::DScopeDebugger::DInfoStreamType:
                dInfo() << "OUT";
                break;
            case dfs::core::DScopeDebugger::DDebugStreamType:
                dDebug() << "OUT";
                break;
            case dfs::core::DScopeDebugger::DWarningStreamType:
                dWarning() << "OUT";
                break;

        }
    }
    else
    {
        switch(outType_)
        {
            case dfs::core::DScopeDebugger::DInfoStreamType:
                dInfo() << "OUT:" <<  message_;
                break;
            case dfs::core::DScopeDebugger::DDebugStreamType:
                dDebug() << "OUT:" <<  message_;
                break;
            case dfs::core::DScopeDebugger::DWarningStreamType:
                dWarning() << "OUT:" <<  message_;
                break;

        }
    }
}
