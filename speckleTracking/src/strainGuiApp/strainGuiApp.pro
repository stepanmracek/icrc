QT += core gui widgets
TARGET = strainGuiApp
TEMPLATE = app

QMAKE_CXXFLAGS+= -std=c++11

LIBS += `pkg-config --libs opencv`
LIBS += -lqwt

LIBS += -L../linalg -llinalg
LIBS += -L../strain -lstrain
LIBS += -L../strainui -lstrainui

INCLUDEPATH += ..

SOURCES += \
    main.cpp
