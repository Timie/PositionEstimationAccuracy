#ifndef RCE_CORE_RTEMPORARYFILEREGISTER_H
#define RCE_CORE_RTEMPORARYFILEREGISTER_H

#include <QMap>


class QTemporaryFile;

namespace rce {
    namespace core {


        /**
         * @brief The RTemporaryFileRegister class does nothing more, just deletes all
         *  added temporary files upon its destruction.
         */
        class RTemporaryFileRegister
        {
        public:
            RTemporaryFileRegister();
            ~RTemporaryFileRegister();


            void addTemporaryFile(const QString &originalName,
                                  QTemporaryFile *file);

            QTemporaryFile* getTemporaryFile(const QString &originalName);


        protected:

            // name of the file -> temporary file
            QMap<QString, QTemporaryFile *> temporaryFiles_;
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RTEMPORARYFILEREGISTER_H
