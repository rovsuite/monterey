#include <QApplication>
#include "mainwindow.h"
#include "extraclasses/Fervor/fvupdater.h"

/*
    Copyright (C) 2012  Chris Konstad (chriskon149@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
