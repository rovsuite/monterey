#include "raspberrypidebugwidget.h"
#include "ui_raspberrypidebugwidget.h"

RaspberryPiDebugWidget::RaspberryPiDebugWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RaspberryPiDebugWidget)
{
    ui->setupUi(this);
}

RaspberryPiDebugWidget::~RaspberryPiDebugWidget()
{
    delete ui;
}

void RaspberryPiDebugWidget::setIp(QHostAddress ip)
{
    ui->labIp->setText(ip.toString());
}

void RaspberryPiDebugWidget::setTibo(int port)
{
    ui->labTiboPort->setText(QString::number(port));
}

void RaspberryPiDebugWidget::setTempCpuC(double temp)
{
    ui->labCpuTemperature->setText(QString::number(temp) + " " + QChar(0x00B0) + "C");  //0x00B0 is the degree char
}

void RaspberryPiDebugWidget::setUptimeSeconds(int uptime)
{
    QString uptimeString;
    unsigned int hours = uptime / (60 * 60);    //convert seconds to hours
    unsigned int minutes = (uptime % (60 * 60)) / (60); //convert to minutes
    unsigned int seconds = ((uptime % (60 * 60)) % (60));   //convert to seconds

    uptimeString.append(QString::number(hours).rightJustified(2, '0'));
    uptimeString.append(":");
    uptimeString.append(QString::number(minutes).rightJustified(2, '0'));
    uptimeString.append(":");
    uptimeString.append(QString::number(seconds).rightJustified(2, '0'));

    ui->labUptime->setText(uptimeString);
}

void RaspberryPiDebugWidget::setUsedMemoryPercentage(double percentage)
{
    ui->pbUsedMem->setValue((int)percentage);
}

void RaspberryPiDebugWidget::setUsedCpuPercentage(double percentage)
{
    ui->pbUsedCpu->setValue((int)percentage);
}
