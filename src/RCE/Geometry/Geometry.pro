INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += _USE_MATH_DEFINES

HEADERS += \
    $$PWD/rce/geometry/RGeometry.h \
    $$PWD/rce/geometry/RCameraPoseEstimation.h

SOURCES += \
    $$PWD/rce/geometry/RGeometry.cpp \
    $$PWD/rce/geometry/RCameraPoseEstimation.cpp
