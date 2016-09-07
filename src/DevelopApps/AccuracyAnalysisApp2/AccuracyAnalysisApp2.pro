TEMPLATE = app
CONFIG += qt c++11 console

QT += xml


TARGET = AccuracyAnalysisApp2
DESTDIR = $$PWD/../bin

RCE_REQUIRED_LIBS += EIGEN OPENCV
CONFIG += RCE_USE_OPENCV_WORLD

QMAKE_CXXFLAGS += /openmp

DEFINES += RCE_ONLY_CORE
DEFINES += RCE_NO_INFO_OUTPUT
DEFINES += RCE_NO_DEBUG_OUTPUT
DEFINES += DFS_COPY_DEBUG_OUTPUT_TO_FILE='"\\\"experiment_output.txt\\\""' DFS_DEBUG_DO_NOT_MIRROR

include(../../RCE/ProjectConfiguration/ProjectConfiguration.pri)
include(../../RCE/Core/Core.pro)
include(../../RCE/Geometry/Geometry.pro)
include(../../RCE/Utility/Utility.pro)
include(../../DFS/DCore/DCore.pri)


include(../../DFS/3rdParty/3rdParty.pro)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD


INCLUDEPATH += $$PWD/../AccuracyAnalysisApp

HEADERS += \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RVirtualScene.h \
    $$PWD/../AccuracyAnalysisApp/rce/uncertainty/RUncertaintyGenerator.h \
    $$PWD/../AccuracyAnalysisApp/rce/uncertainty/RConstantErrorGenerator.h \
    $$PWD/../AccuracyAnalysisApp/rce/uncertainty/RGaussianNosieGenerator.h \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RStatisticsCollector.h \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RVisionExperiment.h \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RVirtualCamera.h \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RVirtualDistortion.h \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/ROpenCVDistortion.h \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/ROpenCVErrDistortion.h \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RCSVOutput.h

SOURCES += \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RVirtualScene.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/uncertainty/RUncertaintyGenerator.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/uncertainty/RConstantErrorGenerator.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/uncertainty/RGaussianNosieGenerator.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RStatisticsCollector.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RVisionExperiment.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RVirtualCamera.cpp \
    $$PWD/main.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RVirtualDistortion.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/ROpenCVDistortion.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/ROpenCVErrDistortion.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RCSVOutput.cpp

