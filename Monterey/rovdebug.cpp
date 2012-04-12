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

    //TODO: Add more debug information
}

ROVDebug::~ROVDebug()
{
    delete ui;
}

void ROVDebug::updateGUI()
{
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    ui->leTOBI->setText(p->controller->txPacket);
    ui->lcdPortTIBO->display(p->controller->getPortTIBO());
    ui->lcdPortTOBI->display(p->controller->getPortTOBI());
}
