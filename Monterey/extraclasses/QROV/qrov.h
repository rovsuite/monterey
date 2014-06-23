#ifndef QROV_H
#define QROV_H

#include <QString>
#include <QList>
#include <QHostAddress>
#include <QUrl>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDateTime>

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
    qint64 msSinceEpoch;
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
        msSinceEpoch = QDateTime::currentMSecsSinceEpoch();

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

//QROV helper functions

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

inline QROVSensor getVoltageSensor(const QROV& rov)
{
    foreach(QROVSensor sensor, rov.sensors)
    {
        if(sensor.name.toLower() == "voltage")
        {
            return sensor;
        }
    }

    return QROVSensor("voltage", "volts", 0);
}

inline QJsonObject getPiDataAsJsonObject(const PiData& data)
{
    QJsonObject object;
    object["tempC"] = data.tempC;
    object["uptimeS"] = data.uptimeS;
    object["ipAddress"] = data.ipAddress.toString();
    object["usedMemory"] = data.usedMemory;
    object["usedCpu"] = data.usedCpu;

    return object;
}

inline QJsonObject getIpVideoFeedAsJsonObject(const IpVideoFeed& feed)
{
    QJsonObject object;
    object["name"] = feed.name;
    object["url"] = feed.url.toString();
    object["autoGenerate"] = feed.autoGenerate;

    return object;
}

//Return the ROV as a JSON object for easy saving
inline QJsonObject getRovAsJsonObject(const QROV& rov)
{
    QJsonObject object;

    QJsonArray motorArray;
    foreach(QROVMotor motor, rov.motors)
    {
        QJsonObject motorObject;
        motorObject["value"] = motor.value;

        motorArray.append(motorObject);
    }
    object["motors"] = motorArray;

    QJsonArray relayArray;
    foreach(QROVRelay relay, rov.relays)
    {
        QJsonObject relayObject;
        relayObject["name"] = relay.name;
        relayObject["enabled"] = relay.enabled;

        relayArray.append(relayObject);
    }
    object["relays"] = relayArray;

    QJsonArray servoArray;
    foreach(QROVServo servo, rov.servos)
    {
        QJsonObject servoObject;
        servoObject["name"] = servo.name;
        servoObject["value"] = servo.value;

        servoArray.append(servoObject);
    }
    object["servos"] = servoArray;

    QJsonArray sensorArray;
    foreach(QROVSensor sensor, rov.sensors)
    {
        QJsonObject sensorObject;
        sensorObject["name"] = sensor.name;
        sensorObject["units"] = sensor.units;
        sensorObject["value"] = sensor.value;

        sensorArray.append(sensorObject);
    }
    object["sensors"] = sensorArray;

    object["version"] = rov.version;
    object["maxDepth"] = rov.maxDepth;
    object["motorLayout"] = rov.motorLayout;
    object["msSinceEpoch"] = rov.msSinceEpoch;

    object["videoFeed"] = getIpVideoFeedAsJsonObject(rov.videoFeed);
    object["piData"] = getPiDataAsJsonObject(rov.piData);

    return object;
}

#endif // QROV_H
