#-------------------------------------------------
#
# Project created by QtCreator 2018-05-23T23:26:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = EF-logo.ico

TARGET = EFRecorder
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    DeviceEnumerator.cpp \
    main.cpp \
    mainwindow.cpp \
    videorecorder.cpp \
    whereami.cpp \
    configwindow.cpp \
    settings.cpp \
    audiorecorder.cpp \
    videomuxing.cpp

HEADERS += \
    DeviceEnumerator.h \
    deviceenumerator.h \
    mainwindow.h \
    videorecorder.h \
    whereami.h \
    configwindow.h \
    settings.h \
    audiorecorder.h \
    videomuxing.h

FORMS += \
        mainwindow.ui \
    configwindow.ui

win32 {
    INCLUDEPATH += C:\opencv\build\include
    LIBS += C:\opencv\build\bin\libopencv_*.dll
}

win32 {
    INCLUDEPATH += C:\bass24\c
    LIBS += -LC:\bass24\c -lbass
}

win32 {
    INCLUDEPATH += C:\basswma24\c
    LIBS += -LC:\basswma24\c -lbasswma
}

win32 {
    INCLUDEPATH += C:\ffmpeg\include
    LIBS += -LC:\ffmpeg\lib -lavcodec -lavformat -lavutil
}


win32: LIBS += -lstrmiids
win32: LIBS += -lole32
win32: LIBS += -loleaut32

DISTFILES +=

RESOURCES += \
    resource.qrc
