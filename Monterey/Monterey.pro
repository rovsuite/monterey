#-------------------------------------------------
#
# Project created by QtCreator 2012-03-15T20:24:49
#
#-------------------------------------------------

QT       += core gui network svg widgets quick webkit

CONFIG += c++11

TARGET = Monterey
VERSION = 4.0
TEMPLATE = app

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
    extraclasses/ConfigParser/configparser.cpp

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
    depends/osx/SDL.framework/Headers/begin_code.h \
    depends/osx/SDL.framework/Headers/close_code.h \
    depends/osx/SDL.framework/Headers/SDL.h \
    depends/osx/SDL.framework/Headers/SDL_active.h \
    depends/osx/SDL.framework/Headers/SDL_audio.h \
    depends/osx/SDL.framework/Headers/SDL_byteorder.h \
    depends/osx/SDL.framework/Headers/SDL_cdrom.h \
    depends/osx/SDL.framework/Headers/SDL_config.h \
    depends/osx/SDL.framework/Headers/SDL_config_dreamcast.h \
    depends/osx/SDL.framework/Headers/SDL_config_macos.h \
    depends/osx/SDL.framework/Headers/SDL_config_macosx.h \
    depends/osx/SDL.framework/Headers/SDL_config_os2.h \
    depends/osx/SDL.framework/Headers/SDL_config_win32.h \
    depends/osx/SDL.framework/Headers/SDL_copying.h \
    depends/osx/SDL.framework/Headers/SDL_cpuinfo.h \
    depends/osx/SDL.framework/Headers/SDL_endian.h \
    depends/osx/SDL.framework/Headers/SDL_error.h \
    depends/osx/SDL.framework/Headers/SDL_events.h \
    depends/osx/SDL.framework/Headers/SDL_getenv.h \
    depends/osx/SDL.framework/Headers/SDL_joystick.h \
    depends/osx/SDL.framework/Headers/SDL_keyboard.h \
    depends/osx/SDL.framework/Headers/SDL_keysym.h \
    depends/osx/SDL.framework/Headers/SDL_loadso.h \
    depends/osx/SDL.framework/Headers/SDL_main.h \
    depends/osx/SDL.framework/Headers/SDL_mouse.h \
    depends/osx/SDL.framework/Headers/SDL_mutex.h \
    depends/osx/SDL.framework/Headers/SDL_name.h \
    depends/osx/SDL.framework/Headers/SDL_opengl.h \
    depends/osx/SDL.framework/Headers/SDL_platform.h \
    depends/osx/SDL.framework/Headers/SDL_quit.h \
    depends/osx/SDL.framework/Headers/SDL_rwops.h \
    depends/osx/SDL.framework/Headers/SDL_stdinc.h \
    depends/osx/SDL.framework/Headers/SDL_syswm.h \
    depends/osx/SDL.framework/Headers/SDL_thread.h \
    depends/osx/SDL.framework/Headers/SDL_timer.h \
    depends/osx/SDL.framework/Headers/SDL_types.h \
    depends/osx/SDL.framework/Headers/SDL_version.h \
    depends/osx/SDL.framework/Headers/SDL_video.h \
    depends/osx/SDL.framework/Versions/A/Headers/begin_code.h \
    depends/osx/SDL.framework/Versions/A/Headers/close_code.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_active.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_audio.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_byteorder.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_cdrom.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_config.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_config_dreamcast.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_config_macos.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_config_macosx.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_config_os2.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_config_win32.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_copying.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_cpuinfo.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_endian.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_error.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_events.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_getenv.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_joystick.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_keyboard.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_keysym.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_loadso.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_main.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_mouse.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_mutex.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_name.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_opengl.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_platform.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_quit.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_rwops.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_stdinc.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_syswm.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_thread.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_timer.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_types.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_version.h \
    depends/osx/SDL.framework/Versions/A/Headers/SDL_video.h \
    depends/osx/SDL.framework/Versions/Current/Headers/begin_code.h \
    depends/osx/SDL.framework/Versions/Current/Headers/close_code.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_active.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_audio.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_byteorder.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_cdrom.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_config.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_config_dreamcast.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_config_macos.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_config_macosx.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_config_os2.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_config_win32.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_copying.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_cpuinfo.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_endian.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_error.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_events.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_getenv.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_joystick.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_keyboard.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_keysym.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_loadso.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_main.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_mouse.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_mutex.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_name.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_opengl.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_platform.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_quit.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_rwops.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_stdinc.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_syswm.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_thread.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_timer.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_types.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_version.h \
    depends/osx/SDL.framework/Versions/Current/Headers/SDL_video.h \
    extraclasses/ConfigParser/configparser.h \
    SDL/begin_code.h \
    SDL/close_code.h \
    SDL/SDL.h \
    SDL/SDL_active.h \
    SDL/SDL_audio.h \
    SDL/SDL_byteorder.h \
    SDL/SDL_cdrom.h \
    SDL/SDL_config.h \
    SDL/SDL_config_dreamcast.h \
    SDL/SDL_config_macos.h \
    SDL/SDL_config_macosx.h \
    SDL/SDL_config_minimal.h \
    SDL/SDL_config_nds.h \
    SDL/SDL_config_os2.h \
    SDL/SDL_config_symbian.h \
    SDL/SDL_config_win32.h \
    SDL/SDL_copying.h \
    SDL/SDL_cpuinfo.h \
    SDL/SDL_endian.h \
    SDL/SDL_error.h \
    SDL/SDL_events.h \
    SDL/SDL_getenv.h \
    SDL/SDL_joystick.h \
    SDL/SDL_keyboard.h \
    SDL/SDL_keysym.h \
    SDL/SDL_loadso.h \
    SDL/SDL_main.h \
    SDL/SDL_mouse.h \
    SDL/SDL_mutex.h \
    SDL/SDL_name.h \
    SDL/SDL_opengl.h \
    SDL/SDL_platform.h \
    SDL/SDL_quit.h \
    SDL/SDL_rwops.h \
    SDL/SDL_stdinc.h \
    SDL/SDL_syswm.h \
    SDL/SDL_thread.h \
    SDL/SDL_timer.h \
    SDL/SDL_types.h \
    SDL/SDL_version.h \
    SDL/SDL_video.h \
    SDL-linux/begin_code.h \
    SDL-linux/close_code.h \
    SDL-linux/SDL.h \
    SDL-linux/SDL_active.h \
    SDL-linux/SDL_audio.h \
    SDL-linux/SDL_byteorder.h \
    SDL-linux/SDL_cdrom.h \
    SDL-linux/SDL_config.h \
    SDL-linux/SDL_cpuinfo.h \
    SDL-linux/SDL_endian.h \
    SDL-linux/SDL_error.h \
    SDL-linux/SDL_events.h \
    SDL-linux/SDL_getenv.h \
    SDL-linux/SDL_joystick.h \
    SDL-linux/SDL_keyboard.h \
    SDL-linux/SDL_keysym.h \
    SDL-linux/SDL_loadso.h \
    SDL-linux/SDL_main.h \
    SDL-linux/SDL_mouse.h \
    SDL-linux/SDL_mutex.h \
    SDL-linux/SDL_name.h \
    SDL-linux/SDL_opengl.h \
    SDL-linux/SDL_platform.h \
    SDL-linux/SDL_quit.h \
    SDL-linux/SDL_rwops.h \
    SDL-linux/SDL_stdinc.h \
    SDL-linux/SDL_syswm.h \
    SDL-linux/SDL_thread.h \
    SDL-linux/SDL_timer.h \
    SDL-linux/SDL_types.h \
    SDL-linux/SDL_version.h \
    SDL-linux/SDL_video.h

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


RESOURCES += \
    resources.qrc

#Linux
unix:!macx{
LIBS += -lSDL
}

#OSX
macx{
ICON = monterey.icns

HEADERS +=     ../Monterey/extraclasses/SDL/SDLMain.h
QMAKE_LFLAGS += -F../Monterey/depends/osx/
LIBS += -framework SDL

OTHER_FILES += \
    ../Monterey/extraclasses/SDL/SDLMain.m
}

#Windows
win32{
RC_FILE = monterey.rc

DEFINES += SDL_WIN
LIBS += -L../Monterey/depends/windows/
LIBS += -lSDL.dll
}

# Fervor autoupdater
!include("../Monterey/extraclasses/Fervor/Fervor.pri") {
   error("Unabled to include Fervor autoupdater.")
}
