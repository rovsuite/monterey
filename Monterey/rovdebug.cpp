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
