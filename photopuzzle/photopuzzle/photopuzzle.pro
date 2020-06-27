QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = photopuzzle
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    pieceslist.cpp

HEADERS  += \
    mainwindow.h \
    pieceslist.h
