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
    opengl.cpp \
    camera.cpp

HEADERS  += mainwindow.h \
    offreader.h \
    opengl.h \
    camera.h

FORMS    += mainwindow.ui
