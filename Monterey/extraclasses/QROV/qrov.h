#ifndef QROV_H
#define QROV_H

//Written by Chris Konstad
//March 2012

#include <QObject>
#include <QList>
#include "qrovmotor.h"
#include "qrovrelay.h"
#include "qrovservo.h"
#include "qrovsensor.h"

class QROV : public QObject
{
    Q_OBJECT
public:
    QROV(int motors, int relays, int servos, QObject *parent);

    QList<QROVMotor*> listMotors;
    QList<QROVRelay*> listRelays;
    QList<QROVServo*> listServos;
    QROVSensor *sensorDepth;
    QROVSensor *sensorCompass;
    QROVSensor *sensorVoltage;
    QROVSensor *sensorOther0;
    QROVSensor *sensorOther1;

signals:

public slots:
    void setNumMotors(int m);
    int getNumMotors();

    void setNumRelays(int r);
    int getNumRelays();

    void setNumServos(int s);
    int getNumServos();

    void setVersion(double v);
    double getVersion();
private:
    int numMotors;
    int numRelays;
    int numServos;
    double version;
};

#endif // QROV_H
