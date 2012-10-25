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

#include "qrovmotor.h"

QROVMotor::QROVMotor(QObject *parent) :
    QObject(parent)
{
    //Initialize values to keep division by zero from occuring by default
    min = 0;
    max = 100;
    value = 50;
}

void QROVMotor::init(int minimum, int maximum, int inputValue)
{
    min = minimum;
    max = maximum;
    value = inputValue;
}

void QROVMotor::setValue(int input)
{
    value = input;
}

int QROVMotor::getValue()
{
    if(value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    else
    {
        return value;
    }
}

void QROVMotor::setMinimum(int minimum)
{
    min = minimum;
}

int QROVMotor::getMinimum()
{
    return min;
}

void QROVMotor::setMaximum(int maximum)
{
    max = maximum;
}

int QROVMotor::getMaximum()
{
    return max;
}

double QROVMotor::getPercentThrust()
{
    double percent;
    percent = (double)(value/(max-min));
    percent = percent * 100.0;
    return percent;
}
