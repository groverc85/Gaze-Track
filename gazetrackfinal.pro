QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gazetrackfinal
TEMPLATE = app

CONFIG += MYMACHINE
MYMACHINE{
    INCLUDEPATH = /usr/local/include
    LIBS += -L/usr/local/lib/
}
INCLUDEPATH = /usr/local/include
LIBS += -lm -lopencv_core -lopencv_highgui -lopencv_video -lopencv_imgproc -lopencv_objdetect


SOURCES +=\
    drawGazeCycles.cpp \
    findEyeCenter.cpp \
    findEyeCorner.cpp \
    helpers.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS  += \
    constants.h \
    drawGazeCycles.h \
    findEyeCenter.h \
    findEyeCorner.h \
    helpers.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui

@CONFIG += MAC_CONFIG

macx {
   QMAKE_MAC_SDK = macosx10.10
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
}

CXXFLAGS += -std=c++0x

DISTFILES += \
    haarcascade_eye_tree_eyeglasses.xml \
    haarcascade_frontalface_alt.xml
