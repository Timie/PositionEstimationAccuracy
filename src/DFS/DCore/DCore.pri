INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dfs/core/DTimeStamp.h \
    $$PWD/dfs/core/DGuardedMap.h \
    $$PWD/dfs/core/DDebug.h \
    $$PWD/dfs/core/DThreadPriorityEscalator.h \
    $$PWD/dfs/core/DScopeDebugger.h \
    $$PWD/dfs/core/DFileDebug.h

SOURCES += \
    $$PWD/dfs/core/DTimeStamp.cpp \
    $$PWD/dfs/core/RGuardedMap.cpp \
    $$PWD/dfs/core/DDebug.cpp \
    $$PWD/dfs/core/DThreadPriorityEscalator.cpp \
    $$PWD/dfs/core/DScopeDebugger.cpp \
    $$PWD/dfs/core/DFileDebug.cpp
