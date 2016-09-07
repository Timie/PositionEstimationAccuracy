#include "DDebug.h"

#include <iostream>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>


void
dfs::core::DDebugInitialiser::
init()
{
    static DDebugInitialiser tmp;
    Q_UNUSED(tmp);
}

dfs::core::DDebugInitialiser::
DDebugInitialiser()
{
#ifdef DFS_COPY_DEBUG_OUTPUT_TO_FILE
    // redirect all debug stuff to file
    QString filePath = DFS_COPY_DEBUG_OUTPUT_TO_FILE;
    if(editDebugOutputFile(filePath).open(QFile::Truncate | QFile::Text | QFile::WriteOnly))
    {
        std::cerr << "Redirecting Debug output to a file \"" << DFS_COPY_DEBUG_OUTPUT_TO_FILE << "\" with full path \"" << QFileInfo(editDebugOutputFile()).absoluteFilePath().toStdString() << "\"" << std::endl;
        // we could open the file - now install the handler
        editFormerMessageHandler() = qInstallMessageHandler(myMessageHandler);
    }
    else
    {
        qDebug() << "Could not open the debug file for writing";
    }
#endif

#ifdef DFS_CUSTOM_DEBUG_FORMAT
    qSetMessagePattern(DFS_CUSTOM_DEBUG_FORMAT);
#endif
}

QFile &
dfs::core::DDebugInitialiser::
editDebugOutputFile(const QString &path)
{
    static QFile file(path);
    return file;
}

void
dfs::core::DDebugInitialiser::
myMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &message)
{
#ifdef DFS_COPY_DEBUG_OUTPUT_TO_FILE
    {
        QTextStream stream(&editDebugOutputFile());
        stream << qFormatLogMessage(type, ctx, message) << "\n";
#ifndef DFS_DEBUG_DO_NOT_FLUSH
        stream.flush();
#endif
    }

#ifndef DFS_DEBUG_DO_NOT_FLUSH
    editDebugOutputFile().flush();
#endif

#endif
#ifndef DFS_DEBUG_DO_NOT_MIRROR
    QtMessageHandler formerMsgHandler = editFormerMessageHandler();
    if(formerMsgHandler != NULL)
    {
        formerMsgHandler(type,
                         ctx,
                         message);
    }
#endif
}

QtMessageHandler &
dfs::core::DDebugInitialiser::
editFormerMessageHandler()
{
    static QtMessageHandler handler = NULL;
    return handler;
}
