#ifndef RASPBERRYPIDEBUGWIDGET_H
#define RASPBERRYPIDEBUGWIDGET_H

#include <QWidget>
#include <QHostAddress>
#include <QTime>

namespace Ui {
class RaspberryPiDebugWidget;
}

class RaspberryPiDebugWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit RaspberryPiDebugWidget(QWidget *parent = 0);
    ~RaspberryPiDebugWidget();

public slots:
    void setIp(QHostAddress ip);
    void setTibo(int port);
    void setTempCpuC(double temp);
    void setUptimeSeconds(int uptime);
    void setUsedMemoryPercentage(double percentage);
    void setUsedCpuPercentage(double percentage);

    
private:
    Ui::RaspberryPiDebugWidget *ui;
    QHostAddress ipAddress;
    int tiboPort;
    double tempCpuC;
    int uptimeSeconds;
    double usedMemoryPercentage;
    double usedCpuPercentage;
};

#endif // RASPBERRYPIDEBUGWIDGET_H
