QT += core gui widgets
TARGET = strainui
TEMPLATE = lib

QMAKE_CXXFLAGS+= -std=c++11

LIBS += `pkg-config --libs opencv`
LIBS += -lqwt
LIBS += -L../linalg -llinalg
LIBS += -L../strain -lstrain


INCLUDEPATH += ..

HEADERS += \
    widgetplot.h \
    widgetspectrogram.h \
    widgetstrainvideo.h \
    widgetanotation.h \
    uiutils.h \
    dialoganotation.h \
    widgetresult.h \
    dialogcreatecoordsystemradial.h \
    dialogvideodataclipmetadata.h \
    modellistofints.h \
    dialogstrainstatistics.h \
    windowanotationmanager.h \
    dialogbeattobeat.h \
    dialogcreatetracker.h \
    dialogimageprocessing.h \
    dialogshapemodel.h \
    graphicsviewwithevents.h \
    dialogsaveplot.h \
    widgetrainbow.h

SOURCES += \
    widgetspectrogram.cpp \
    widgetstrainvideo.cpp \
    widgetanotation.cpp \
    dialoganotation.cpp \
    widgetresult.cpp \
    dialogcreatecoordsystemradial.cpp \
    dialogvideodataclipmetadata.cpp \
    dialogstrainstatistics.cpp \
    windowanotationmanager.cpp \
    dialogbeattobeat.cpp \
    dialogcreatetracker.cpp \
    dialogimageprocessing.cpp \
    dialogshapemodel.cpp \
    graphicsviewwithevents.cpp \
    widgetplot.cpp \
    dialogsaveplot.cpp \
    widgetrainbow.cpp \

FORMS += \
    widgetstrainvideo.ui \
    dialoganotation.ui \
    dialogcreatecoordsystemradial.ui \
    dialogvideodataclipmetadata.ui \
    dialogstrainstatistics.ui \
    windowanotationmanager.ui \
    dialogbeattobeat.ui \
    dialogcreatetracker.ui \
    dialogimageprocessing.ui \
    dialogshapemodel.ui \
    dialogsaveplot.ui

RESOURCES += \
    resources.qrc
