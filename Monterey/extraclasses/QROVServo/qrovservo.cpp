#include "qrovservo.h"

QROVServo::QROVServo(QObject *parent) :
    QObject(parent)
{
    value = 0;
    lastValue = value;
    min = 0;
    max = 1;    //keep division by zero from happening automatically
    name = "Servo";
    hatUp = 1;
    hatDown = 4;
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

void QROVServo::setHatUp(int up)
{
    hatUp = up;
}

void QROVServo::setHatDown(int down)
{
    hatDown = down;
}

int QROVServo::getHatUp()
{
    return hatUp;
}

int QROVServo::getHatDown()
{
    return hatDown;
}
