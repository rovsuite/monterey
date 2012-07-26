#include <QtGui/QApplication>
#include "mainwindow.h"
#include "extraclasses/Fervor/fvupdater.h"

#undef main

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Monterey");
    QApplication::setApplicationVersion("2.0.3 Beta");
    QApplication::setOrganizationName("ROV-Suite");
    QApplication::setOrganizationDomain("http://sourceforge.net/p/rov-suite");

    FvUpdater::sharedUpdater()->SetFeedURL("https://dl.dropbox.com/u/4649414/ROV-Suite/Appcast.xml");
    FvUpdater::sharedUpdater()->CheckForUpdatesSilent();
    MainWindow w;
    w.show();

    return a.exec();
}
