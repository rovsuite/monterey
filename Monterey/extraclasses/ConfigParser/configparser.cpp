#include "configparser.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QFile>
#include <QtDebug>
#include "qrov.h"

ConfigParser::ConfigParser(const QString& file, QObject *parent) :
    QObject(parent)
{
    mFile = file;
}

bool ConfigParser::parseRov(QROV& rov) const
{
    QByteArray fileContents = readFile();
    if(fileContents.isEmpty())
        return false;

    QJsonDocument configDoc(QJsonDocument::fromJson(fileContents));
    QJsonObject baseObj = configDoc.object();

    QJsonArray jsonSensors = baseObj["sensors"].toArray();
    QJsonArray jsonRelays = baseObj["relays"].toArray();
    QJsonArray jsonServos = baseObj["servos"].toArray();
    QJsonValue jsonMotorLayout = baseObj["motorLayout"];
    QJsonValue jsonMaxDepth = baseObj["maxDepth"];

    if(jsonSensors.size() == 0 ||
            jsonRelays.size() == 0 ||
            jsonServos.size() == 0 ||
            jsonMotorLayout.isUndefined() ||
            jsonMaxDepth.isUndefined())
        return false;

    //TODO: READ IP VIDEO FEEDS?

    rov.relays.clear();
    rov.servos.clear();
    rov.sensors.clear();
    rov.motors.clear();

    for(int i=0; i<jsonRelays.count(); i++)
    {
        rov.relays.append(QROVRelay(jsonRelays[i].toString(), false));
    }

    for(int i=0; i<jsonServos.count(); i++)
    {
        rov.servos.append(QROVServo(jsonServos[i].toString(), 0));
    }

    for(int i=0; i<jsonSensors.count(); i++)
    {
        if(!jsonSensors[i].toObject().contains("units") ||
                !jsonSensors[i].toObject().contains("name"))
            return false;

        rov.sensors.append(QROVSensor(jsonSensors[i].toObject()["name"].toString(),
                           jsonSensors[i].toObject()["units"].toString(), 0));
    }

    if(jsonMotorLayout.toString() == "vector")
    {
        rov.motorLayout = vectorDrive;
        for(int i=0; i<6; i++)
        {
            rov.motors.append(QROVMotor(1500));
        }
    }
    else if(jsonMotorLayout.toString() == "tank")
    {
        rov.motorLayout = tankDrive;
        for(int i=0; i<3; i++)
        {
            rov.motors.append(QROVMotor(1500));
        }
    }
    else
    {
        qWarning() << "Motor layout: " << jsonMotorLayout.toString() << " not defined!";
        return false;
    }

    rov.maxDepth = jsonMaxDepth.toDouble(100);

    return true;
}

void ConfigParser::setFile(const QString &filename)
{
    mFile = filename;
}

QByteArray ConfigParser::readFile() const
{
    QFile loadFile(mFile);
    QByteArray res;

    if(!loadFile.open(QIODevice::ReadOnly))
    {
        qCritical("Couldn't open ROV configuration file");
    }
    else
    {
        res = loadFile.readAll();
        loadFile.close();
    }

    return res;
}
