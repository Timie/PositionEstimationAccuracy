#ifndef DFS_CORE_DFILEDEBUG_H
#define DFS_CORE_DFILEDEBUG_H

#include <QDebug>

class QString;


namespace dfs {
    namespace core {

        class DFileDebug
        {
        public:

            // if file could not be opened, QtDebugMsg stream is returned
            static QDebug getFileStream(const QString& fileName);

            static void flush(const QString& fileName = QString());
        };

    } // namespace core
} // namespace dfs

#ifdef qInfo
// there is support for qInfo in Qt
#ifdef RCE_NO_INFO_OUTPUT
#ifdef QT_NO_QINFO_MACRO
#define dFileInfo(M) QT_NO_QINFO_MACRO
#else
#define dFileInfo(M) QT_NO_QDEBUG_MACRO
#endif
#else
#define dFileInfo(M) dfs::core::DFileDebug::getFileStream(M)
#endif
#else
// there is no support for qInfo in Qt - defaults to qDebug
#ifdef RCE_NO_INFO_OUTPUT
#define dFileInfo(M) QT_NO_QDEBUG_MACRO
#else
#define dFileInfo(M) dfs::core::DFileDebug::getFileStream(M)
#endif
#endif

#ifdef RCE_NO_DEBUG_OUTPUT
#define dFileDebug(M) QT_NO_QDEBUG_MACRO
#else
#define dFileDebug(M) dfs::core::DFileDebug::getFileStream(M)
#endif

#ifdef RCE_NO_WARNING_OUTPUT
#define qFileWarning(M) QT_NO_QWARNING_MACRO
#else
#define qFileWarning(M) fs::core::DFileDebug::getFileStream(M)
#endif

#define dFileOutput(M) dfs::core::DFileDebug::getFileStream(M)

#define dFileCritical(M) dfs::core::DFileDebug::getFileStream(M)


#endif // DFS_CORE_DFILEDEBUG_H
