#include "qrovsensor.h"

QROVSensor::QROVSensor(QObject *parent) :
    QObject(parent)
{
    min = 0;
    max = 1;
    val = 0;
    threshold = 0;
    warnAbove = true;
    units = "units";
    name = "sensor";
}

void QROVSensor::setMin(double m)
{
    min = m;
}

double QROVSensor::getMin()
{
    return min;
}

void QROVSensor::setMax(double m)
{
    max = m;
}

double QROVSensor::getMax()
{
    return max;
}

void QROVSensor::setValue(double v)
{
    val = v;
    if(warnAbove)   //if warn above is enabled
    {
        if(val >= threshold)
        {
            emit thresholdReached();
        }
    }
    else    //if warn below
    {
        if(val <= threshold)
        {
            emit thresholdReached();
        }
    }
}

double QROVSensor::getValue()
{
    return val;
}

void QROVSensor::setThreshold(double t)
{
    threshold = t;
}

double QROVSensor::getThreshold()
{
    return threshold;
}

double QROVSensor::getPercent()
{
    double percent = val/(max-min);
    return percent;
}

void QROVSensor::setWarnAbove(bool b)
{
    warnAbove = b;
}

bool QROVSensor::getWarnAbove()
{
    return warnAbove;
}

void QROVSensor::setWarnBelow(bool b)
{
    warnAbove = !b;
}

bool QROVSensor::getWarnBelow()
{
    return !warnAbove;
}

void QROVSensor::setUnits(QString u)
{
    units = u;
}

QString QROVSensor::getUnits()
{
    return units;
}

void QROVSensor::setName(QString n)
{
    name = n;
}

QString QROVSensor::getName()
{
    return name;
}
