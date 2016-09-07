#ifndef RCE_CORE_RPATHMANAGER_H
#define RCE_CORE_RPATHMANAGER_H

class QString;
class QStringList;
class QFileInfo;

namespace rce {
    namespace core {

        class RTemporaryFileRegister;

        class RPathManager
        {
        public:

            // all methods are not reentrant nor thread safe

            // NOTE: This description is valid only for Qt4.8
            /**
             * @brief getFilePath analyzed given file path in following manner and returns most likely path to file.
             * Manager checks whether the file path is Qt Recource file path. If so, the temporary file is created
             * with contents of referenced Qt resource (binary copy) and path to the created temporary file is returned.
             * Else manager checks whether the file is presented in given filePath. If so, the method
             * returns full path to file in system native format (fe: on Windows with backslashes).
             * If file is not present in given filePath, manager looks for file in search paths in following order:
             * 1.) Current Working Directory (QDir::currentPath())
             * 2.) Application Dir Path (QCoreApplication::applicationDirPath())
             * 3.) Application Data Location (QDesktopServices::DataLocation)
             * 4.) User provided search paths in order they were provided
             *
             * The search is carried out both with directory path included in filePath (if any) and without, former preferred.
             *
             * If file could not be found, the original filePath is returned.
             */
            static QString getFilePath(const QString &filePath);

            /**
             * @brief addSearchPath
             * @param path - path to be added to user provided search paths
             */
            static void addSearchPath(const QString &path);


            /**
             * @brief getSearchPaths
             * @return All search paths as described in getFilePath in that order. If given search path
             * could not be retrieved from system, it is skipped (not contained)
             */
            static const QStringList &getSearchPaths();


            static QString getWritableAppDataPath(const QString &candidatePath);

            static bool isFileWritable(const QFileInfo &f);

        protected:
            static QStringList &accessStaticSearchPaths();
            static QStringList initializeSearchPaths();
            static rce::core::RTemporaryFileRegister &accessStaticTempFileRegister();
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RPATHMANAGER_H
