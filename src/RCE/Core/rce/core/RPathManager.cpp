#include "RPathManager.h"

#include <QString>
#include <QStringList>
#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QFile>
#include <QDebug>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QStandardPaths>
#else
#include <QAbstractFileEngine>
#endif

#include "rce/core/RTemporaryFileRegister.h"
#include "dfs/core/DDebug.h"


QString
rce::core::RPathManager::
getFilePath(const QString &filePath)
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if(!QFileInfo(filePath).isNativePath())
    {
        // filePath is path to Qt resource - we attemt to retrieve temporary file
        QTemporaryFile *localFile = accessStaticTempFileRegister().getTemporaryFile(filePath);
        if(localFile == NULL)
        { // we have not created temporary file before for this resource, so we have to create it now
            localFile = QTemporaryFile::createNativeFile(filePath);

            accessStaticTempFileRegister().addTemporaryFile(filePath,
                                                            localFile);

            if(localFile->open())
                localFile->close();


            //qDebug() << "Temp File for" << filePath << "was created as" << QFileInfo(*localFile).absoluteFilePath();
        }


        return QFileInfo(*localFile).absoluteFilePath();
    }
#else
    QFile fileTst(filePath);
    if((fileTst.fileEngine() != NULL) &&
       (!(fileTst.fileEngine()->fileFlags(QAbstractFileEngine::LocalDiskFlag).testFlag(QAbstractFileEngine::LocalDiskFlag))))
    {
        // filePath is path to Qt resource - we attemt to retrieve temporary file
        QTemporaryFile *localFile = accessStaticTempFileRegister().getTemporaryFile(filePath);
        if(localFile == NULL)
        { // we have not created temporary file before for this resource, so we have to create it now
            localFile = QTemporaryFile::createLocalFile(filePath);

            accessStaticTempFileRegister().addTemporaryFile(filePath,
                                                            localFile);

            if(localFile->open())
                localFile->close();


            //qDebug() << "Temp File for" << filePath << "was created as" << QFileInfo(*localFile).absoluteFilePath();
        }


        return QFileInfo(*localFile).absoluteFilePath();
    }
#endif
    else if(QFile::exists(filePath))
    {
        return QDir::toNativeSeparators(QDir::cleanPath(QDir(filePath).absolutePath()));
    }
    else
    {
        QString qtFilePath = QDir::fromNativeSeparators(filePath);

        QString qtFileName = QFileInfo(qtFilePath).fileName();

        bool searchWithPath = ((!QDir(qtFilePath).isAbsolute()) &&
                               (qtFilePath != qtFileName));

        for(int i = 0; i < accessStaticSearchPaths().size(); ++i)
        {
            if(searchWithPath)
            {
                QString wholeSearchPath = accessStaticSearchPaths()[i] + "/" + qtFilePath;
                if(QFile::exists(wholeSearchPath))
                {
                    return QDir::toNativeSeparators(QDir::cleanPath(QDir(wholeSearchPath).absolutePath()));
                }
            }

            QString fileSearchPath = accessStaticSearchPaths()[i] + "/" + qtFileName;
            if(QFile::exists(fileSearchPath))
            {
                return QDir::toNativeSeparators(QDir::cleanPath(QDir(fileSearchPath).absolutePath()));
            }
        }

        return filePath;
    }
}

void
rce::core::RPathManager::
addSearchPath(const QString &path)
{
    if(!path.isEmpty())
        accessStaticSearchPaths().push_back(QDir::toNativeSeparators(QDir::cleanPath(QDir(path).absolutePath())));
}

const QStringList &
rce::core::RPathManager::
getSearchPaths()
{
    return accessStaticSearchPaths();
}

QString
rce::core::RPathManager::
getWritableAppDataPath(const QString &candidateFile)
{
    // if the file path is full, prefer it if it is writable
    {
        QFileInfo f(candidateFile);
        if(f.isAbsolute() && isFileWritable(f))
        {
            dDebug() << "Using absolute path";
            return candidateFile;
        }
    }

    // check app dir
    {
        QDir appDir(QCoreApplication::applicationDirPath());
        QFileInfo appDirFile(appDir, QFileInfo(candidateFile).fileName());
        if(isFileWritable(appDirFile))
        {
            dDebug() << "Using app dir path";
            return appDirFile.absoluteFilePath();
        }
    }

    // check app data location
    {
        dDebug() << "Using app data location path";
        QDir standardDataDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)); // always should return writable location
        standardDataDir.mkpath(standardDataDir.absolutePath());
        return QFileInfo(standardDataDir, QFileInfo(candidateFile).fileName()).absoluteFilePath();
    }
}

bool
rce::core::RPathManager::
isFileWritable(const QFileInfo &f)
{
    if(f.exists())
    {
        return f.isWritable();// file exist and can be modified
    }
    else if(QFileInfo(f.absolutePath()).isWritable())
    {
        return true;
    }
    else
    {
#ifdef Q_OS_LINUX
        return false;
#else
        // on win, sometimes you may not have permision for the folder, but you
        // still may have ability to create the file
        QFile file(f.absoluteFilePath());
        if(file.open(QFile::WriteOnly)) // experiment to open file for writing
        {
            file.close();
            file.remove();
            return true;
        }
        else
        {
            return false;
        }
#endif
    }
}

QStringList &
rce::core::RPathManager::
accessStaticSearchPaths()
{
    static QStringList searchPaths = initializeSearchPaths();
    return searchPaths;
}

QStringList
rce::core::RPathManager::
initializeSearchPaths()
{
    QStringList newSearchPaths;
    QString cwdPath = QDir::currentPath();
    if(!cwdPath.isEmpty())
        newSearchPaths.push_back(QDir::toNativeSeparators(QDir::cleanPath(QDir(cwdPath).absolutePath())));

    QString exePath = QCoreApplication::applicationDirPath();
    if(!exePath.isEmpty())
        newSearchPaths.push_back(QDir::toNativeSeparators(QDir::cleanPath(QDir(exePath).absolutePath())));


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    newSearchPaths += QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    newSearchPaths += QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
    newSearchPaths += QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    newSearchPaths += QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation);
#else

    QString dataPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if(!dataPath.isEmpty())
        newSearchPaths.push_back(QDir::toNativeSeparators(QDir::cleanPath(QDir(dataPath).absolutePath())));

#endif
    return newSearchPaths;
}

rce::core::RTemporaryFileRegister &
rce::core::RPathManager::
accessStaticTempFileRegister()
{
    static rce::core::RTemporaryFileRegister tempFileRegister;
    return tempFileRegister;
}
