#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int version = 2.0;

    activityMonitor = new QActivityMonitor(this);
    activityMonitor->setTextEdit(ui->teLog);
    activityMonitor->display("Monterey started...");
    QString versionDisp("Version: ");
    versionDisp.append(QString::number(version));
    activityMonitor->display(versionDisp);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout())); //show the about window
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));  //exit the application
    connect(ui->actionDebug, SIGNAL(triggered()), this, SLOT(showDebug())); //show the debug window

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showAbout()
{
    dialogAbout = new ROVAbout(this);
    dialogAbout->exec();
}

void MainWindow::showDebug()
{
    dialogDebug = new ROVDebug(this);
    dialogDebug->show();
}
