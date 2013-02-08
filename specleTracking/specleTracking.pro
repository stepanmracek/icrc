QT += core gui
TARGET = specleTracking
TEMPLATE = app

HEADERS += \
    src/linalg/vecf.h \
    src/linalg/spline.h \
    src/linalg/procrustes.h \
    src/linalg/pca.h \
    src/linalg/common.h \
    src/strain/statisticalshapemodel.h \
    src/strain/anotationdata.h \
    src/linalg/projectionbase.h \
    src/linalg/backprojectionbase.h \
    src/strain/shapetracker.h \
    src/strain/pointtrackerbase.h \
    src/strain/pointtrackeropticalflow.h \
    src/linalg/metrics.h \
    src/strain/pointtrackerdistance.h \
    src/strain/shapenormalizer.h \
    src/strain/coordsystem.h \
    src/linalg/ica.h \
    src/linalg/serializableobject.h \
    src/linalg/serialization.h \
    src/strain/anotation.h \
    src/test/test.h \
    src/strain/statisticalshapechanges.h \
    src/strain/strainstatistics.h \
    src/ui/widgetstrainstatistics.h \
    src/ui/widgetplot.h \
    src/ui/widgetspectrogram.h \
    src/strain/videodatabase.h \
    src/strain/videodataclip.h \
    src/ui/widgetstrainvideo.h \
    src/strain/pointtrackerneighbouropticalflow.h \
    src/strain/imagefilter.h \
    src/strain/strainresultprocessing.h \
    src/ui/widgetanotation.h \
    src/ui/uiutils.h \
    src/ui/widgetanotationmanager.h \
    src/strain/shapeprocessing.h \
    src/strain/strain.h \
    src/strain/longitudinalstrain.h \
    src/ui/dialoganotation.h \
    src/ui/widgetresult.h \
    src/ui/dialogcreatecoordsystemradial.h \
    src/ui/dialogvideodataclipmetadata.h \
    src/ui/modellistofints.h

SOURCES += \
    src/linalg/vecf.cpp \
    src/linalg/spline.cpp \
    src/linalg/procrustes.cpp \
    src/linalg/pca.cpp \
    src/strain/statisticalshapemodel.cpp \
    src/test/main.cpp \
    src/strain/shapetracker.cpp \
    src/strain/pointtrackeropticalflow.cpp \
    src/linalg/metrics.cpp \
    src/strain/pointtrackerdistance.cpp \
    src/strain/coordsystem.cpp \
    src/linalg/ica.cpp \
    src/linalg/serialization.cpp \
    src/strain/anotation.cpp \
    src/strain/statisticalshapechanges.cpp \
    src/strain/strainstatistics.cpp \
    src/ui/widgetstrainstatistics.cpp \
    src/ui/widgetplot.cpp \
    src/ui/widgetspectrogram.cpp \
    src/strain/videodatabase.cpp \
    src/strain/videodataclip.cpp \
    src/ui/widgetstrainvideo.cpp \
    src/strain/pointtrackerneighbouropticalflow.cpp \
    src/strain/imagefilter.cpp \
    src/strain/shapenormalizer.cpp \
    src/strain/strainresultprocessing.cpp \
    src/ui/widgetanotation.cpp \
    src/ui/widgetanotationmanager.cpp \
    src/strain/shapeprocessing.cpp \
    src/strain/strain.cpp \
    src/strain/longitudinalstrain.cpp \
    src/ui/dialoganotation.cpp \
    src/ui/widgetresult.cpp \
    src/ui/dialogcreatecoordsystemradial.cpp \
    src/ui/dialogvideodataclipmetadata.cpp \
    src/strain/pointtrackerbase.cpp

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
    src/ui/widgetstrainstatistics.ui \
    src/ui/widgetstrainvideo.ui \
    src/ui/widgetanotationmanager.ui \
    src/ui/dialoganotation.ui \
    src/ui/dialogcreatecoordsystemradial.ui \
    src/ui/dialogvideodataclipmetadata.ui

RESOURCES += \
    src/ui/resources.qrc
