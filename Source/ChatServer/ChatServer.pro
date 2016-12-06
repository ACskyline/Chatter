#-------------------------------------------------
#
# Project created by QtCreator 2016-06-24T10:46:30
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChatServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    server.cpp \
    tcpclientsocket.cpp

HEADERS  += mainwindow.h \
    server.h \
    tcpclientsocket.h

FORMS    += mainwindow.ui
