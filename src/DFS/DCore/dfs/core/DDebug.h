#ifndef DFS_CORE_DDEBUGLOGGER_H
#define DFS_CORE_DDEBUGLOGGER_H

// enable message log context when the cutsom debug format is required (this may not be enough in case the QDebug was included before this header)
#ifdef DFS_CUSTOM_DEBUG_FORMAT
#ifndef QT_MESSAGELOGCONTEXT
#define QT_MESSAGELOGCONTEXT
#endif
#endif

#include <QDebug>

//#define DFS_COPY_DEBUG_OUTPUT_TO_FILE "debug.log" // path to a file
//#define DFS_CUSTOM_DEBUG_FORMAT "[%{if-category}%{category}: %{endif}%{time yyyyMMdd h:mm:ss.zzz}] %{file}:%{line} - %{message}" // message patter (see qSetMessagePattern in Qt)

//#define RCE_NO_INFO_OUTPUT
//#define RCE_NO_DEBUG_OUTPUT
//#define RCE_NO_WARNING_OUTPUT


class QFile;

namespace dfs {
    namespace core {

        class DDebugInitialiser
        {
        public:
            /**
             *  This method automatically redirects all q_info/debug/warning/critical/fatal
             *  according to defines DFS_COPY_OUTPUT_TO_FILE and
             */
            static void init();

        protected:
            DDebugInitialiser(); // does all the initialisation...

            static QFile &editDebugOutputFile(const QString &path = QString());

            static void myMessageHandler(QtMsgType, const QMessageLogContext &, const QString &);

            static QtMessageHandler &editFormerMessageHandler();
        };

    } // namespace core
} // namespace dfs


// FIXME: Maybe even for RCE_NO_***_OUTPUT call init

#ifdef qInfo
// there is support for qInfo in Qt
#ifdef RCE_NO_INFO_OUTPUT
#ifdef QT_NO_QINFO_MACRO
#define dInfo QT_NO_QINFO_MACRO
#else
#define dInfo QT_NO_QDEBUG_MACRO
#endif
#else
#define dInfo dfs::core::DDebugInitialiser::init();qInfo
#endif
#else
// there is no support for qInfo in Qt - defaults to qDebug
#ifdef RCE_NO_INFO_OUTPUT
#define dInfo QT_NO_QDEBUG_MACRO
#else
#define dInfo dfs::core::DDebugInitialiser::init();qDebug
#endif
#endif

#ifdef RCE_NO_DEBUG_OUTPUT
#define dDebug QT_NO_QDEBUG_MACRO
#else
#define dDebug dfs::core::DDebugInitialiser::init();qDebug
#endif

#ifdef RCE_NO_WARNING_OUTPUT
#define qWarning QT_NO_QWARNING_MACRO
#else
#define dWarning dfs::core::DDebugInitialiser::init();qWarning
#endif


#define dCritical dfs::core::DDebugInitialiser::init();qCritical
#define dFatal dfs::core::DDebugInitialiser::init();qFatal



#endif // DFS_CORE_DDEBUGLOGGER_H
