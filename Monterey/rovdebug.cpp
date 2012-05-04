#include "rovdebug.h"
#include "ui_rovdebug.h"
#include "mainwindow.h"

ROVDebug::ROVDebug(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVDebug)
{
    ui->setupUi(this);
    updateTimer = new QTimer(this);
    updateTimer->setInterval(50);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateGUI()));
    updateTimer->start();

    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    ui->lcdPortTIBO->display(p->controller->getPortTIBO());
    ui->lcdPortTOBI->display(p->controller->getPortTOBI());
    connect(p->controller, SIGNAL(sentPacket(QString)), this, SLOT(displayTOBIPacket(QString)));
    connect(p->controller, SIGNAL(receivedPacket(QString)), this, SLOT(displayTIBOPacket(QString)));

    //TODO: Add more debug information
}

ROVDebug::~ROVDebug()
{
    delete ui;
}

void ROVDebug::updateGUI()
{
    //TODO: Any debug stuff to add here on a timer?
}

void ROVDebug::displayTIBOPacket(QString toDisp)
{
    //toDisp = packet;
    ui->leTIBO->setText(toDisp);
}

void ROVDebug::displayTOBIPacket(QString toDisp)
{
    //toDisp = packet;
    ui->leTOBI->setText(toDisp);
}
