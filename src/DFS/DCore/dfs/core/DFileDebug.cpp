#include "DFileDebug.h"

#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QFile>
#include <QDebug>
#include <QReadWriteLock>
#include <QMutex>

#include "dfs/core/DDebug.h"

// FUNCTIONS
QMap<QString, QSharedPointer<QFile>> &
editDebugFiles()
{
    static QMap<QString, QSharedPointer<QFile>> res;
    return res;
}


const QMap<QString, QSharedPointer<QFile>> &
getDebugFiles()
{
    return editDebugFiles();
}

QReadWriteLock &getFilesLock()
{
    static QReadWriteLock accessLock;
    return accessLock;
}



// METHODS

QDebug
dfs::core::DFileDebug::
getFileStream(const QString &fileName)
{
    QReadWriteLock &accessLock = getFilesLock();

    accessLock.lockForRead();

    auto it = getDebugFiles().find(fileName);
    if(it == getDebugFiles().end())
    {
        accessLock.unlock();
        static QMutex createMutex;
        createMutex.lock(); // only one thread can be creating the file
        // check it once again
        accessLock.lockForRead();
        auto it2 = getDebugFiles().find(fileName);
        if(it2 != getDebugFiles().end())
        {
            // the data is created, just return it
            createMutex.unlock();
            auto tmp = it.value().data();
            accessLock.unlock();
            return QDebug(tmp);
        }
        accessLock.unlock();

        // there are really no files like that - create it

        accessLock.lockForWrite();
        // no such file - try to open it
        QSharedPointer<QFile> debFile(new QFile(fileName));
        if(debFile->open(QFile::WriteOnly | QFile::Truncate))
        {
            editDebugFiles()[fileName] = debFile;
            createMutex.unlock();
            accessLock.unlock();
            return QDebug(debFile.data());
        }
        else
        {
            // could not creeate
            createMutex.unlock();
            accessLock.unlock();

            return QDebug(QtDebugMsg);
        }
    }
    else
    {
        auto tmp = it.value().data();
        accessLock.unlock();
        return QDebug(tmp);
    }
}

void
dfs::core::DFileDebug::
flush(const QString &fileName)
{
    // TODO: Finish this!
}
