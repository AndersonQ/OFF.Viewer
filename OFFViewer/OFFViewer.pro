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
    trackball.cpp \
    light.cpp \
    material.cpp

HEADERS  += mainwindow.h \
    offreader.h \
    camera.h \
    opengl.h \
    trackball.h \
    light.h \
    material.h

FORMS    += mainwindow.ui

RESOURCES += \
    shaders.qrc

OTHER_FILES += \
    Shaders/fshader.glsl \
    Shaders/vshader.glsl \
    Shaders/fshader.Flat.glsl \
    Shaders/vshader.Flat.glsl \
    Shaders/fshader.Gouraud.glsl \
    Shaders/vshader.Gouraud.glsl \
    Shaders/fshader.Phong.glsl \
    Shaders/vshader.Phong.glsl \
    Shaders/fshader.Phong.Half.glsl \
    Shaders/vshader.Phong.Half.glsl \
    Shaders/fshader.Cartoon.glsl \
    Shaders/vshader.Cartoon.glsl \
    Shaders/fshader.Simple.Tex.Mapping.glsl \
    Shaders/vshader.Simple.Tex.Mapping.glsl \
    Shaders/fshader.Cube.Mapping.glsl \
    Shaders/vshader.Cube.Mapping.glsl
