#-------------------------------------------------
#
# Project created by QtCreator 2016-05-17T20:10:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Partiton-Editor
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    harddiskmanager.cpp \
    deletepartitionwindow.cpp \
    resizepartitionwindow.cpp \
    createpartitionwindow.cpp

HEADERS  += mainwindow.h \
    harddiskmanager.h \
    partitionstructure.h \
    deletepartitionwindow.h \
    resizepartitionwindow.h \
    createpartitionwindow.h

FORMS    += mainwindow.ui \
    deletepartitionwindow.ui \
    resizepartitionwindow.ui \
    createpartitionwindow.ui
