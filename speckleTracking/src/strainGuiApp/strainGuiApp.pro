QT += core gui widgets
TARGET = speckleTracking-gui
TEMPLATE = app

QMAKE_CXXFLAGS+= -std=c++11

DESTDIR = ../

LIBS += `pkg-config --libs opencv`
LIBS += -lqwt

LIBS += -L../linalg -llinalg
LIBS += -L../strain -lstrain
LIBS += -L../strainui -lstrainui

INCLUDEPATH += ..

SOURCES += \
    main.cpp
