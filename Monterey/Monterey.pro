#-------------------------------------------------
#
# Project created by QtCreator 2012-03-15T20:24:49
#
#-------------------------------------------------

QT       += core gui network svg declarative

TARGET = Monterey
VERSION = 2.0
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    ../Monterey/extraclasses/QActivityMonitor/qactivitymonitor.cpp \
    ../Monterey/extraclasses/QBoolMonitor/qboolmonitor.cpp \
    ../Monterey/extraclasses/QROVMotor/qrovmotor.cpp \
    ../Monterey/extraclasses/QROVRelay/qrovrelay.cpp \
    ../Monterey/extraclasses/QROVServo/qrovservo.cpp \
    rovdebug.cpp \
    rovsettings.cpp \
    rovmappings.cpp \
    rovabout.cpp \
    extraclasses/QScale/qscale.cpp \
    extraclasses/QNeedleIndicator/qneedleindicator.cpp \
    extraclasses/QLedIndicator/qledindicator.cpp \
    extraclasses/QROV/qrov.cpp \
    extraclasses/QROVController/qrovcontroller.cpp \
    extraclasses/QROVSensor/qrovsensor.cpp \
    extraclasses/QJoystick/qjoystick.cpp \
    extraclasses/QVectorDrive2/qvectordrive2.cpp \
    extraclasses/QCustomPlot/qcustomplot.cpp \

HEADERS  += mainwindow.h \
    ../Monterey/extraclasses/QActivityMonitor/qactivitymonitor.h \
    ../Monterey/extraclasses/QBoolMonitor/qboolmonitor.h \
    ../Monterey/extraclasses/QROVMotor/qrovmotor.h \
    ../Monterey/extraclasses/QROVRelay/qrovrelay.h \
    ../Monterey/extraclasses/QROVServo/qrovservo.h \
    rovdebug.h \
    rovsettings.h \
    rovmappings.h \
    rovabout.h \
    extraclasses/QScale/qscale.h \
    extraclasses/QNeedleIndicator/qneedleindicator.h \
    extraclasses/QLedIndicator/qledindicator.h \
    extraclasses/QROV/qrov.h \
    extraclasses/QROVController/qrovcontroller.h \
    extraclasses/QROVSensor/qrovsensor.h \
    extraclasses/QJoystick/qjoystick.h \
    extraclasses/QVectorDrive2/qvectordrive2.h \
    extraclasses/QCustomPlot/qcustomplot.h

FORMS    += mainwindow.ui \
    rovdebug.ui \
    rovsettings.ui \
    rovmappings.ui \
    rovabout.ui \

INCLUDEPATH += ../Monterey/extraclasses/QActivityMonitor/ \
                ../Monterey/extraclasses/QBoolMonitor/ \
                ../Monterey/extraclasses/QROVMotor/ \
                ../Monterey/extraclasses/QROVRelay/ \
                ../Monterey/extraclasses/QROVServo/ \
                ../Monterey/extraclasses/QScale/ \
                ../Monterey/extraclasses/QNeedleIndicator/ \
                ../Monterey/extraclasses/QLedIndicator/ \
                ../Monterey/extraclasses/QROV/ \
                ../Monterey/extraclasses/QROVController/ \
                ../Monterey/extraclasses/QROVSensor/ \
                ../Monterey/extraclasses/QJoystick/


RESOURCES += \
    resources.qrc

macx{
ICON = monterey.icns

HEADERS +=     ../Monterey/extraclasses/SDL/SDLMain.h
QMAKE_LFLAGS += -F../Monterey/depends/osx/
LIBS += -framework SDL

OTHER_FILES += \
    ../Monterey/extraclasses/SDL/SDLMain.m
}

win32{
RC_FILE = monterey.rc

DEFINES += SDL_WIN
LIBS += -lSDL.dll
}

OTHER_FILES += \
    resources/ROV.png \
    resources/hsi_face.svg.png \
    resources/any_casing.svg.png \
    resources/Compass.qml \
    resources/HeadingIndicator.qml \

# Fervor autoupdater
!include("../Monterey/extraclasses/Fervor/Fervor.pri") {
    error("Unabled to include Fervor autoupdater.")
}
