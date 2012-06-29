#include <QtGui/QApplication>
#include "mainwindow.h"

#undef main

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Monterey");
    QApplication::setApplicationVersion("2.0.2 Beta");
    QApplication::setOrganizationName("ROV-Suite");
    QApplication::setOrganizationDomain("http://sourceforge.net/p/rov-suite");
    MainWindow w;
    w.show();

    return a.exec();
}
