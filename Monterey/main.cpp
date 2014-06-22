#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"
#include "extraclasses/Fervor/fvupdater.h"

#undef main

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Monterey");
    QApplication::setApplicationVersion(VERSION);
    QApplication::setOrganizationName("ROV-Suite");

    a.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    a.setPalette(darkPalette);
    a.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    QApplication::setOrganizationDomain("http://github.com/rovsuite");

    FvUpdater::sharedUpdater()->SetFeedURL("https://dl.dropboxusercontent.com/u/4649414/ROV-Suite/Appcast.xml");
    FvUpdater::sharedUpdater()->CheckForUpdatesSilent();
    MainWindow w;
    w.show();

    return a.exec();
}
