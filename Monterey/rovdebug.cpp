#include "rovdebug.h"
#include "ui_rovdebug.h"
#include "mainwindow.h"
#include "extraclasses/RaspberryPiDebugWidget/raspberrypidebugwidget.h"

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
}

ROVDebug::~ROVDebug()
{
    delete ui;
}

void ROVDebug::updateGUI()
{
    //Any debug stuff to add here on a timer
    MainWindow *p = dynamic_cast<MainWindow *>(this->parentWidget());
    QHostAddress address = p->controller->rov->piData->ipAddress();
    int port = p->controller->getPortRpiTibo();
    double tempC = p->controller->rov->piData->tempC();
    int uptime = p->controller->rov->piData->uptimeS();
    double usedMem = p->controller->rov->piData->usedMemory();
    double usedCpu = p->controller->rov->piData->usedCpu();

    ui->rPiDataWidget->setIp(address);
    ui->rPiDataWidget->setTibo(port);
    ui->rPiDataWidget->setTempCpuC(tempC);
    ui->rPiDataWidget->setUptimeSeconds(uptime);
    ui->rPiDataWidget->setUsedMemoryPercentage(usedMem);
    ui->rPiDataWidget->setUsedCpuPercentage(usedCpu);
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
