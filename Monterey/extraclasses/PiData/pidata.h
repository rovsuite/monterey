#ifndef PIDATA_H
#define PIDATA_H

#include <QObject>
#include <QHostAddress>

class PiData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double tempC READ tempC WRITE setTempC NOTIFY tempCChanged)
    Q_PROPERTY(int uptimeMs READ uptimeMs WRITE setUptimeMs)
    Q_PROPERTY(QHostAddress* ipAddress READ ipAddress WRITE setIpAddress)
    Q_PROPERTY(double usedMemory READ usedMemory WRITE setUsedMemory NOTIFY usedMemoryChanged)
    Q_PROPERTY(double usedCpu READ usedCpu WRITE setUsedCpu NOTIFY usedCpuChanged)

public:
    explicit PiData(QObject *parent = 0);
    
    double tempC() const;

    int uptimeMs() const;

    QHostAddress* ipAddress() const;

    double usedMemory() const;

    double usedCpu() const;

signals:
    
    void tempCChanged(double arg);

    void usedMemoryChanged(double arg);

    void usedCpuChanged(double arg);

public slots:

    void setTempC(double arg);

    void setUptimeMs(int arg);

    void setIpAddress(QHostAddress* arg);

    void setUsedMemory(double arg);

    void setUsedCpu(double arg);

private:
    double m_tempC;
    int m_uptimeMs;
    QHostAddress* m_ipAddress;
    double m_usedMemory;
    double m_usedCpu;
};

#endif // PIDATA_H
