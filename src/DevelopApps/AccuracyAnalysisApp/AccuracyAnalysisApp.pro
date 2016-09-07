

TEMPLATE = app
CONFIG += qt c++11 console

QT += xml

TARGET = AccuracyAnalysisAppParallel
DESTDIR = ../bin

RCE_REQUIRED_LIBS += EIGEN OPENCV

QMAKE_CXXFLAGS += /openmp

DEFINES += RCE_ONLY_CORE
DEFINES += RCE_NO_INFO_OUTPUT
DEFINES += RCE_NO_DEBUG_OUTPUT
DEFINES += DFS_COPY_DEBUG_OUTPUT_TO_FILE='"\\\"D:/ADAM/Work/Rce/git/develop/DevelopApps/experiment_output.txt\\\""' DFS_DEBUG_DO_NOT_MIRROR

include(../../RCE/ProjectConfiguration/ProjectConfiguration.pri)
include(../../RCE/Core/Core.pro)
include(../../RCE/Geometry/Geometry.pro)
include(../../RCE/Utility/Utility.pro)
include(../../DFS/DCore/DCore.pri)


include(../../DFS/3rdParty/3rdParty.pro)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/rce/accuracy/RVirtualScene.h \
    $$PWD/rce/uncertainty/RUncertaintyGenerator.h \
    $$PWD/rce/uncertainty/RConstantErrorGenerator.h \
    $$PWD/rce/uncertainty/RGaussianNosieGenerator.h \
    $$PWD/rce/accuracy/RStatisticsCollector.h \
    $$PWD/rce/accuracy/RVisionExperiment.h \
    $$PWD/rce/accuracy/RVirtualCamera.h \
    $$PWD/rce/accuracy/RVirtualDistortion.h \
    $$PWD/rce/accuracy/ROpenCVDistortion.h \
    $$PWD/rce/accuracy/ROpenCVErrDistortion.h \
    $$PWD/rce/accuracy/RCSVOutput.h

SOURCES += \
    $$PWD/rce/accuracy/RVirtualScene.cpp \
    $$PWD/rce/uncertainty/RUncertaintyGenerator.cpp \
    $$PWD/rce/uncertainty/RConstantErrorGenerator.cpp \
    $$PWD/rce/uncertainty/RGaussianNosieGenerator.cpp \
    $$PWD/rce/accuracy/RStatisticsCollector.cpp \
    $$PWD/rce/accuracy/RVisionExperiment.cpp \
    $$PWD/rce/accuracy/RVirtualCamera.cpp \
    $$PWD/main.cpp \
    $$PWD/rce/accuracy/RVirtualDistortion.cpp \
    $$PWD/rce/accuracy/ROpenCVDistortion.cpp \
    $$PWD/rce/accuracy/ROpenCVErrDistortion.cpp \
    $$PWD/rce/accuracy/RCSVOutput.cpp
