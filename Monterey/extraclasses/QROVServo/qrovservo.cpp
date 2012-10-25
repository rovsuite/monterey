/*
    Copyright (C) 2012  Chris Konstad (chriskon149@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    buttonUp = 3;
    buttonDown = 0;
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

void QROVServo::setButtonUp(int up)
{
    buttonUp = up;
}

void QROVServo::setButtonDown(int down)
{
    buttonDown = down;
}

int QROVServo::getButtonUp()
{
    return buttonUp;
}

int QROVServo::getButtonDown()
{
    return buttonDown;
}
