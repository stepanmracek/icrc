QT += core
TARGET = linalg
TEMPLATE = lib

QMAKE_CXXFLAGS+= -std=c++11

LIBS += `pkg-config --libs opencv`

HEADERS += \
    vecf.h \
    spline.h \
    procrustes.h \
    pca.h \
    common.h \
    projectionbase.h \
    backprojectionbase.h \
    metrics.h \
    ica.h \
    serializableobject.h \
    serialization.h \
    frequencymodulation.h \
    shapecomparer.h \
    widthmodulation.h

SOURCES += \
    vecf.cpp \
    spline.cpp \
    procrustes.cpp \
    pca.cpp \
    metrics.cpp \
    ica.cpp \
    serialization.cpp \
    frequencymodulation.cpp \
    shapecomparer.cpp \
    widthmodulation.cpp
