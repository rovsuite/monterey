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

public:
    explicit PiData(QObject *parent = 0);
    
    double tempC() const;

    int uptimeMs() const;

    QHostAddress* ipAddress() const;

signals:
    
    void tempCChanged(double arg);

public slots:

void setTempC(double arg);

void setUptimeMs(int arg);


void setIpAddress(QHostAddress* arg);

private:
    double m_tempC;
    int m_uptimeMs;
    QHostAddress* m_ipAddress;
};

#endif // PIDATA_H
