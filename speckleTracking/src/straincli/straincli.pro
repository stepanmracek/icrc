QT += core gui widgets
TARGET = speckleTracking-cli
TEMPLATE = app

QMAKE_CXXFLAGS+= -std=c++11

DESTDIR = ../

LIBS += `pkg-config --libs opencv`
LIBS += -lqwt

LIBS += -L../linalg -llinalg
LIBS += -L../strain -lstrain
LIBS += -L../strainui -lstrainui

INCLUDEPATH += ..

HEADERS += \
    batchstraintracker.h \
    createmodel.h

SOURCES += \
    batchstraintracker.cpp \
    main.cpp \
    createmodel.cpp
