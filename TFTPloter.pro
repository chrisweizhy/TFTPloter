#-------------------------------------------------
#
# Project created by QtCreator 2017-05-20T12:34:21
#
#-------------------------------------------------

QT       += core gui network
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = TFTPloter
TEMPLATE = app


INCLUDEPATH += network


SOURCES += main.cpp\
        MainWindow.cpp \
    network/TCPServer.cpp \
    network/DataParser.cpp \
    FormPlot.cpp \
    qcustomplot.cpp \
    PagePlotSingleCurve.cpp \
    network/DataProvider.cpp \
    tester.cpp

HEADERS  += MainWindow.h \
    network/TCPServer.h \
    network/DataParser.h \
    FormPlot.h \
    qcustomplot.h \
    PagePlotSingleCurve.h \
    network/DataProvider.h \
    tester.h

FORMS    += MainWindow.ui

include(loki/loki.pri)
