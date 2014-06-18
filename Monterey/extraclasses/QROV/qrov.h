#ifndef QROV_H
#define QROV_H

#include <QString>
#include <QList>

enum MotorLayout { vectorDrive, tankDrive };

struct QROVMotor
{
    int value;
    QROVMotor(int v)
    {
        this->value = v;
    }
};

struct QROVRelay
{
    QString name;
    bool enabled;

    QROVRelay(const QString& name, bool enabled)
    {
        this->name = name;
        this->enabled = enabled;
    }
};

struct QROVServo
{
    QString name;
    int value;

    QROVServo(const QString& name, int v)
    {
        this->name = name;
        this->value = v;
    }
};

struct QROVSensor
{
    QString name;
    QString units;
    double value;

    QROVSensor(const QString& name, const QString& units, double v)
    {
        this->name = name;
        this->units = units;
        this->value = v;
    }
};

class IpVideoFeed;
class PiData;

struct QROV
{
    double version;
    MotorLayout motorLayout;
    QList<IpVideoFeed*> videoFeeds;
    PiData *piData;
    QList<QROVMotor> motors;
    QList<QROVRelay> relays;
    QList<QROVServo> servos;
    QList<QROVSensor> sensors;

    QROV(double v, IpVideoFeed *feed, PiData *pi,
            int numMotors, int numRelays,
            int numServos, int numSensors)
    {
        version = v;

        videoFeeds.clear();
        videoFeeds.append(feed);

        piData = pi;

        motors.clear();
        for(int i=0; i<numMotors; i++)
            motors.append(QROVMotor(1500));

        relays.clear();
        for(int i=0; i<numRelays; i++)
            relays.append(QROVRelay("Relay" + QString::number(i), false));

        servos.clear();
        for(int i=0; i<numServos; i++)
            servos.append(QROVServo("Servo" + QString::number(i), 0));

        sensors.clear();
        for(int i=0; i<numSensors; i++)
            sensors.append(QROVSensor("Sensor" + QString::number(i), "N/A", 0)); 
    }

    QROV()
    {
        QROV(0, NULL, NULL, 0, 0, 0, 0);
    }
};

#endif // QROV_H
