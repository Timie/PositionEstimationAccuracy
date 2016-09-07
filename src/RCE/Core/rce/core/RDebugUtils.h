#ifndef RCE_CORE_RDEBUGUTILS_H
#define RCE_CORE_RDEBUGUTILS_H

#include <QtGlobal>

namespace rce {
    namespace core {

        class RDebugUtils
        {
        public:

            static void setDebugObjectID(quint32 value) {debugObjectID_ = value;}
            static void resetDebugObjectID() {debugObjectID_ = std::numeric_limits<quint32>::max();}
            static bool isDebugObject(quint32 objectID);



        protected:
            static quint32 debugObjectID_;
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RDEBUGUTILS_H
