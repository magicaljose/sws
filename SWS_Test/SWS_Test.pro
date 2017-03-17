#-------------------------------------------------
#
# Project created by QtCreator 2017-03-10T14:39:37
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_sws_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += tst_sws_test.cpp \
    ../SWS/card.cpp \
    ../SWS/command.cpp \
    ../SWS/console.cpp \
    ../SWS/game.cpp \
    ../SWS/klondike.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../SWS/card.h \
    ../SWS/command.h \
    ../SWS/console.h \
    ../SWS/game.h \
    ../SWS/klondike.h
