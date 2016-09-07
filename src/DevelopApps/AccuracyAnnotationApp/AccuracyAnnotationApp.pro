TEMPLATE = app
CONFIG += qt c++11

QT += xml gui widgets xml

TARGET = AccuracyAnalysisApp
DESTDIR = ../bin

RCE_REQUIRED_LIBS += EIGEN OPENCV

DEFINES += RCE_ONLY_CORE
DEFINES += RCE_NO_INFO_OUTPUT
#DEFINES += RCE_NO_DEBUG_OUTPUT
DEFINES += DFS_COPY_DEBUG_OUTPUT_TO_FILE='"\\\"experiment_output_basic.txt\\\""' DFS_DEBUG_DO_NOT_MIRROR

include(../../RCE/ProjectConfiguration/ProjectConfiguration.pri)
include(../../RCE/Core/Core.pro)
include(../../RCE/Geometry/Geometry.pro)
include(../../RCE/Utility/Utility.pro)
include(../../RCE/Gui/GuiBase.pro)
include(../../DFS/DCore/DCore.pri)


include(../../DFS/3rdParty/3rdParty.pro)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD


INCLUDEPATH += $$PWD/../AccuracyAnalysisApp #for RStatisticsCollector

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/rce/gui/RDistanceWidget.cpp \
    $$PWD/rce/accuracy/RDistanceMatrix.cpp \
    $$PWD/rce/gui/RDistanceMatrixScene.cpp \
    $$PWD/rce/gui/RDistanceAnnotationWidget.cpp \
    $$PWD/rce/gui/RPointClickingScene.cpp \
    $$PWD/rce/gui/RImageSequencePointAnnotationWidget.cpp \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RStatisticsCollector.cpp \
    $$PWD/rce/accuracy/RRealExperimentAnalyzer.cpp

HEADERS += \
    $$PWD/rce/gui/RDistanceWidget.h \
    $$PWD/rce/accuracy/RDistanceMatrix.h \
    $$PWD/rce/gui/RDistanceMatrixScene.h \
    $$PWD/rce/gui/RDistanceAnnotationWidget.h \
    $$PWD/rce/gui/RPointClickingScene.h \
    $$PWD/rce/gui/RImageSequencePointAnnotationWidget.h \
    $$PWD/../AccuracyAnalysisApp/rce/accuracy/RStatisticsCollector.h \
    $$PWD/rce/accuracy/RRealExperimentAnalyzer.h
