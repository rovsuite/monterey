#include "qrovservo.h"

QROVServo::QROVServo(QObject *parent) :
    QObject(parent)
{
    value = 0;
    lastValue = value;
    min = 0;
    max = 1;    //keep division by zero from happening automatically
    name = "Servo";
}

QString QROVServo::getName()
{
    return name;
}

void QROVServo::setName(QString n)
{
    name = n;
}

int QROVServo::getValue()
{
    return value;
}

void QROVServo::setValue(int v)
{
    value = v;
    if(lastValue != value)
    {
        emit valueChanged(value);
        double percent;
        percent = (double)(value/(max-min));
        percent = percent * 100.0;
        emit valueChanged(percent);
    }
}

int QROVServo::getMin()
{
    return min;
}

void QROVServo::setMin(int m)
{
    min = m;
}

int QROVServo::getMax()
{
    return max;
}

void QROVServo::setMax(int m)
{
    max = m;
}

double QROVServo::getPercentRotation()
{
    double percent;
    percent = (double)(value/(max-min));
    percent = percent * 100.0;
    return percent;
}
