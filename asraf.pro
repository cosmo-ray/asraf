#-------------------------------------------------
#
# Project created by QtCreator 2013-02-11T15:38:36
#
#-------------------------------------------------

QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = asraf
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11


#QMAKE_LFLAGS += -static-libgcc
win32: LIBS +=  -L$$PWD/lib/ -lavdevice -lavformat -lavcodec -lavutil -lswscale
unix:LIBS +=  -L/usr/lib -lavdevice -lavformat -lavcodec -lavutil -lswscale
#win32: PRE_TARGETDEPS += $$PWD/lib/avformat.lib $$PWD/lib/avcodec.lib $$PWD/lib/avutil.lib $$PWD/lib/avdevice.lib $$PWD/lib/swscale.lib

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

SOURCES += main.cpp\
        mainwindow.cpp\
        Media.cpp\
        core.cpp

HEADERS  += mainwindow.hh \
            Media.hh \
            core.hh
