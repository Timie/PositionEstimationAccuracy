#include "RTemporaryFileRegister.h"

#include <QTemporaryFile>

namespace rce {
    namespace core {

        RTemporaryFileRegister::RTemporaryFileRegister()
        {
        }

        RTemporaryFileRegister::~RTemporaryFileRegister()
        {
            foreach(QTemporaryFile *file, temporaryFiles_)
            {
                delete file;
            }
        }

        void RTemporaryFileRegister::addTemporaryFile(const QString &originalName,
                                                      QTemporaryFile *file)
        {
            temporaryFiles_.insertMulti(originalName, file);
        }

        QTemporaryFile *RTemporaryFileRegister::getTemporaryFile(const QString &originalName)
        {
            auto it = temporaryFiles_.find(originalName);
            if(it != temporaryFiles_.end())
            {
                return it.value();
            }
            else
            {
                return NULL;
            }
        }

    } // namespace core
} // namespace rce
