#-------------------------------------------------
#
# Project created by QtCreator 2012-03-15T20:24:49
#
#-------------------------------------------------

QT += core gui network svg widgets quick webkit printsupport

CONFIG += c++11

VERSION = 4.0
TEMPLATE = app
TARGET = Monterey

DEFINES += VERSION=\\\"$$VERSION\\\"

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
           extraclasses/QROVController/qrovcontroller.cpp \
           extraclasses/QROVSensor/qrovsensor.cpp \
           extraclasses/QJoystick/qjoystick.cpp \
           extraclasses/QVectorDrive2/qvectordrive2.cpp \
           extraclasses/QCustomPlot/qcustomplot.cpp \
           extraclasses/DepthTape/depthtape.cpp \
           extraclasses/DiveTimer/divetimer.cpp \
           extraclasses/IpVideoFeedSettingsWidget/ipvideofeedsettingswidget.cpp \
           extraclasses/Compass/compass.cpp \
           extraclasses/RaspberryPiDebugWidget/raspberrypidebugwidget.cpp \
           extraclasses/LedIndicator/ledindicator.cpp \
           extraclasses/UdpCapture/udpcapture.cpp \
           extraclasses/ConfigParser/configparser.cpp \
           extraclasses/QJoystickInterface/qjoystickinterface.cpp

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
            extraclasses/QROV/qrov.h \
            extraclasses/QROVController/qrovcontroller.h \
            extraclasses/QROVSensor/qrovsensor.h \
            extraclasses/QJoystick/qjoystick.h \
            extraclasses/QVectorDrive2/qvectordrive2.h \
            extraclasses/QCustomPlot/qcustomplot.h \
            extraclasses/DepthTape/depthtape.h \
            extraclasses/DiveTimer/divetimer.h \
            extraclasses/IpVideoFeedSettingsWidget/ipvideofeedsettingswidget.h \
            extraclasses/Compass/compass.h \
            extraclasses/RaspberryPiDebugWidget/raspberrypidebugwidget.h \
            extraclasses/LedIndicator/ledindicator.h \
            extraclasses/UdpCapture/udpcapture.h \
            extraclasses/ConfigParser/configparser.h \
            extraclasses/QJoystickInterface/qjoystickinterface.h

FORMS    += mainwindow.ui \
            rovdebug.ui \
            rovsettings.ui \
            rovmappings.ui \
            rovabout.ui \
            extraclasses/IpVideoFeedSettingsWidget/ipvideofeedsettingswidget.ui \
            extraclasses/RaspberryPiDebugWidget/raspberrypidebugwidget.ui

INCLUDEPATH += ../Monterey/extraclasses/QActivityMonitor/ \
               ../Monterey/extraclasses/QBoolMonitor/ \
               ../Monterey/extraclasses/QROVMotor/ \
               ../Monterey/extraclasses/QROVRelay/ \
               ../Monterey/extraclasses/QROVServo/ \
               ../Monterey/extraclasses/QLedIndicator/ \
               ../Monterey/extraclasses/QROV/ \
               ../Monterey/extraclasses/QROVController/ \
               ../Monterey/extraclasses/QROVSensor/ \
               ../Monterey/extraclasses/QJoystick/

RESOURCES += resources.qrc

release: DESTDIR = build/release
debug: DESTDIR = build/debug

UI_DIR = $$DESTDIR/.ui
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
OBJECTS_DIR = $$DESTDIR/.obj

# Linux
unix:!macx{
    LIBS += -lSDL
}

# Mac OSX
macx{
    ICON = monterey.icns
    QMAKE_LFLAGS += -F../Monterey/depends/osx/
    LIBS += -framework SDL
    HEADERS += ../Monterey/extraclasses/SDL/SDLMain.h
    OTHER_FILES += ../Monterey/extraclasses/SDL/SDLMain.m
}

# Windows
win32{
    DEFINES += SDL_WIN
    RC_FILE = monterey.rc
}

# Windows, MinGW
win32-g++* {
}

# Windows, MSVC
win32-msvc* {
}

# Fervor autoupdater
!include("../Monterey/extraclasses/Fervor/Fervor.pri") {
    error("Unabled to include Fervor autoupdater.")
}
