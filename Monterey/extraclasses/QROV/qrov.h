#ifndef QROV_H
#define QROV_H

#include <QString>
#include <QList>
#include <QHostAddress>
#include <QUrl>

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

struct PiData
{
    double tempC;
    int uptimeS;
    QHostAddress ipAddress;
    double usedMemory;
    double usedCpu;

    PiData(double tempC, int uptimeS, QHostAddress ip, double usedMem, double usedCpu)
    {
        this->tempC = tempC;
        this->uptimeS = uptimeS;
        this->ipAddress = ip;
        this->usedMemory = usedMem;
        this->usedCpu = usedCpu;
    }

    PiData()
    {
        PiData(0, 0, QHostAddress::LocalHost, 0, 0);
    }
};

struct IpVideoFeed
{
    QString name;
    QUrl url;
    bool autoGenerate;

    IpVideoFeed(QString name, QUrl url, bool autoGenerate)
    {
        this->name = name;
        this->url = url;
        this->autoGenerate = autoGenerate;
    }

    IpVideoFeed()
    {
        IpVideoFeed("", QUrl(""), false);
    }
};

struct QROV
{
    double version;
    double maxDepth;
    MotorLayout motorLayout;
    IpVideoFeed videoFeed;
    PiData piData;
    QList<QROVMotor> motors;
    QList<QROVRelay> relays;
    QList<QROVServo> servos;
    QList<QROVSensor> sensors;

    QROV(double v, IpVideoFeed feed, PiData pi,
            int numMotors, int numRelays,
            int numServos, int numSensors, double maxDepth)
    {
        version = v;
        videoFeed = feed;
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

        this->maxDepth = maxDepth;
    }

    QROV()
    {
        QROV(0, IpVideoFeed("main", QUrl("http://127.0.0.1"), false), PiData(), 0, 0, 0, 0, 100);
    }
};

inline QROVSensor getDepthSensor(const QROV& rov)
{
    foreach(QROVSensor sensor, rov.sensors)
    {
        if(sensor.name.toLower() == "depth")
        {
            return sensor;
        }
    }

    return QROVSensor("depth", "m", 0);
}

inline QROVSensor getHeadingSensor(const QROV& rov)
{
    foreach(QROVSensor sensor, rov.sensors)
    {
        if(sensor.name.toLower() == "heading" ||
                sensor.name.toLower() == "compass")
        {
            return sensor;
        }
    }

    return QROVSensor("heading", "degrees", 0);
}

#endif // QROV_H
