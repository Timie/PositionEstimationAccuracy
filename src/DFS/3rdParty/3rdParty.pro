INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include($$PWD/QtOpenCV/QtOpenCV.pri)
include($$PWD/Theia/Theia.pri)

HEADERS += \
    $$PWD/dirent.h \
    $$PWD/RPP.h \
    $$PWD/Rpoly.h

SOURCES += \
    $$PWD/RPP.cpp \
    $$PWD/Rpoly.cpp
