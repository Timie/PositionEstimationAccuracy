#include "RDebugUtils.h"



quint32 rce::core::RDebugUtils::debugObjectID_ = std::numeric_limits<quint32>::max();

bool
rce::core::RDebugUtils::
isDebugObject(quint32 objectID)
{
    if(debugObjectID_ == std::numeric_limits<quint32>::max())
    {
        return false;
    }
    else
    {
        return debugObjectID_ == objectID;
    }
}


