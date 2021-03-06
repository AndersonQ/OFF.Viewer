#-------------------------------------------------
#
# Project created by QtCreator 2012-03-22T13:32:45
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = OFFViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    offreader.cpp \
    camera.cpp \
    opengl.cpp \
    trackball.cpp

HEADERS  += mainwindow.h \
    offreader.h \
    camera.h \
    opengl.h \
    trackball.h

FORMS    += mainwindow.ui

RESOURCES += \
    shaders.qrc

OTHER_FILES += \
    fshader.glsl \
    vshader.glsl
