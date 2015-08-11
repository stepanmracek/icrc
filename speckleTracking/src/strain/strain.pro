QT += core widgets
TARGET = strain
TEMPLATE = lib

QMAKE_CXXFLAGS+= -std=c++11

LIBS += `pkg-config --libs opencv`
LIBS += -L../linalg -llinalg

INCLUDEPATH += ..

HEADERS += \
    statisticalshapemodel.h \
    anotationdata.h \
    shapetracker.h \
    pointtrackerbase.h \
    pointtrackeropticalflow.h \
    pointtrackerdistance.h \
    shapenormalizer.h \
    coordsystem.h \
    anotation.h \
    strainstatistics.h \
    videodatabase.h \
    videodataclip.h \
    pointtrackerneighbouropticalflow.h \
    imagefilter.h \
    strainresultprocessing.h \
    shapeprocessing.h \
    strain.h \
    longitudinalstrain.h \
    batchtestingold.h \
    strainclassifier.h \
    beatdetector.h

SOURCES += \
    statisticalshapemodel.cpp \
    shapetracker.cpp \
    pointtrackeropticalflow.cpp \
    pointtrackerdistance.cpp \
    coordsystem.cpp \
    anotation.cpp \
    strainstatistics.cpp \
    videodatabase.cpp \
    videodataclip.cpp \
    pointtrackerneighbouropticalflow.cpp \
    imagefilter.cpp \
    shapenormalizer.cpp \
    strainresultprocessing.cpp \
    shapeprocessing.cpp \
    strain.cpp \
    longitudinalstrain.cpp \
    pointtrackerbase.cpp \
    batchtestingold.cpp \
    strainclassifier.cpp \
    beatdetector.cpp
