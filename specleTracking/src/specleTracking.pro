QT += core gui
TARGET = specleTracking
TEMPLATE = app

HEADERS += \
    linalg/vecf.h \
    linalg/spline.h \
    linalg/procrustes.h \
    linalg/pca.h \
    linalg/common.h \
    strain/statisticalshapemodel.h \
    strain/anotationdata.h \
    linalg/projectionbase.h \
    linalg/backprojectionbase.h \
    strain/shapetracker.h \
    strain/pointtrackerbase.h \
    strain/pointtrackeropticalflow.h \
    linalg/metrics.h \
    strain/pointtrackerdistance.h \
    strain/shapenormalizer.h \
    strain/coordsystem.h \
    linalg/ica.h \
    linalg/serializableobject.h \
    linalg/serialization.h \
    strain/anotation.h \
    test/test.h \
    strain/statisticalshapechanges.h \
    strain/strainstatistics.h \
    ui/widgetplot.h \
    ui/widgetspectrogram.h \
    strain/videodatabase.h \
    strain/videodataclip.h \
    ui/widgetstrainvideo.h \
    strain/pointtrackerneighbouropticalflow.h \
    strain/imagefilter.h \
    strain/strainresultprocessing.h \
    ui/widgetanotation.h \
    ui/uiutils.h \
    ui/widgetanotationmanager.h \
    strain/shapeprocessing.h \
    strain/strain.h \
    strain/longitudinalstrain.h \
    ui/dialoganotation.h \
    ui/widgetresult.h \
    ui/dialogcreatecoordsystemradial.h \
    ui/dialogvideodataclipmetadata.h \
    ui/modellistofints.h \
    ui/dialogstrainstatistics.h \
    ui/windowanotationmanager.h

SOURCES += \
    linalg/vecf.cpp \
    linalg/spline.cpp \
    linalg/procrustes.cpp \
    linalg/pca.cpp \
    strain/statisticalshapemodel.cpp \
    test/main.cpp \
    strain/shapetracker.cpp \
    strain/pointtrackeropticalflow.cpp \
    linalg/metrics.cpp \
    strain/pointtrackerdistance.cpp \
    strain/coordsystem.cpp \
    linalg/ica.cpp \
    linalg/serialization.cpp \
    strain/anotation.cpp \
    strain/statisticalshapechanges.cpp \
    strain/strainstatistics.cpp \
    ui/widgetplot.cpp \
    ui/widgetspectrogram.cpp \
    strain/videodatabase.cpp \
    strain/videodataclip.cpp \
    ui/widgetstrainvideo.cpp \
    strain/pointtrackerneighbouropticalflow.cpp \
    strain/imagefilter.cpp \
    strain/shapenormalizer.cpp \
    strain/strainresultprocessing.cpp \
    ui/widgetanotation.cpp \
    ui/widgetanotationmanager.cpp \
    strain/shapeprocessing.cpp \
    strain/strain.cpp \
    strain/longitudinalstrain.cpp \
    ui/dialoganotation.cpp \
    ui/widgetresult.cpp \
    ui/dialogcreatecoordsystemradial.cpp \
    ui/dialogvideodataclipmetadata.cpp \
    strain/pointtrackerbase.cpp \
    ui/dialogstrainstatistics.cpp \
    ui/windowanotationmanager.cpp

INCLUDEPATH += src

win32 {
    LIBS += `pkg-config --libs opencv`
}
unix {
    LIBS += `pkg-config --libs opencv`
    LIBS += -lqwt

    # VTK
    #LIBS += -L/usr/lib/vtk-5.10/ -lvtkRendering
    #LIBS += -L/usr/lib/vtk-5.10/ -lvtkGraphics
    #LIBS += -L/usr/lib/vtk-5.10/ -lvtkHybrid
    #LIBS += -L/usr/lib/vtk-5.10/ -lvtkImaging
    #LIBS += -L/usr/lib/vtk-5.10/ -lvtkIO
    #LIBS += -L/usr/lib/vtk-5.10/ -lvtkFiltering
    #LIBS += -L/usr/lib/vtk-5.10/ -lvtkCommon
    #INCLUDEPATH += /usr/include/vtk-5.10
}

FORMS += \
    ui/widgetstrainvideo.ui \
    ui/widgetanotationmanager.ui \
    ui/dialoganotation.ui \
    ui/dialogcreatecoordsystemradial.ui \
    ui/dialogvideodataclipmetadata.ui \
    ui/dialogstrainstatistics.ui \
    ui/windowanotationmanager.ui

RESOURCES += \
    ui/resources.qrc
