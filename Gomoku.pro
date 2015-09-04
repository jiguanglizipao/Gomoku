#-------------------------------------------------
#
# Project created by QtCreator 2015-08-23T19:04:48
#
#-------------------------------------------------

QT       += core gui\
            multimedia\
            network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gomoku
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gamescene.cpp \
    gameview.cpp \
    ipdialog.cpp

HEADERS  += mainwindow.h \
    gamescene.h \
    gameview.h \
    ipdialog.h

FORMS  += mainwindow.ui \
    ipdialog.ui

RESOURCES += res.qrc
