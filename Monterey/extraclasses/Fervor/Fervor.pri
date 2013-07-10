QT += core gui webkit network widgets webkitwidgets

DEFINES += FV_APP_NAME=\\\"$$TARGET\\\"
DEFINES += FV_APP_VERSION=\\\"$$VERSION\\\"


# FIXME unit tests
#DEFINES += FV_DEBUG=1
#DEPENDPATH += "$$PWD/tests/"
#INCLUDEPATH += "$$PWD/tests/"
#CONFIG += qtestlib
#SOURCES += tests/fvversioncomparatortest.cpp
#HEADERS += tests/fvversioncomparatortest.h



DEPENDPATH += "$$PWD"
INCLUDEPATH += "$$PWD"

SOURCES += $$PWD/fvupdatewindow.cpp \
        $$PWD/fvupdater.cpp \
        $$PWD/fvversioncomparator.cpp \
        $$PWD/fvplatform.cpp \
        $$PWD/fvignoredversions.cpp \
        $$PWD/fvavailableupdate.cpp \
        $$PWD/fvupdateconfirmdialog.cpp

HEADERS += $$PWD/fvupdatewindow.h \
        $$PWD/fvupdater.h \
        $$PWD/fvversioncomparator.h \
        $$PWD/fvplatform.h \
        $$PWD/fvignoredversions.h \
        $$PWD/fvavailableupdate.h \
        $$PWD/fvupdateconfirmdialog.h

FORMS += $$PWD/fvupdatewindow.ui \
        $$PWD/fvupdateconfirmdialog.ui

TRANSLATIONS += fervor_lt.ts
CODECFORTR = UTF-8
