#-------------------------------------------------
#
# Project created by QtCreator 2016-11-19T18:54:21
#
#-------------------------------------------------

QT       += core gui network winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SendMessage
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    settingwindow.cpp \
    process.cpp \
    encrypt.cpp \
    load_dll.cpp \
    sendmessage.cpp

HEADERS  += mainwindow.h \
    settingwindow.h \
    encrypt.h \
    load_dll.h

FORMS    += mainwindow.ui \
    setting.ui

RESOURCES += \
    res.qrc \
    icon.qrc

DISTFILES += \
    myapp.rc

DISTFILES += \
    myapp.rc

RC_FILE = myapp.rc
