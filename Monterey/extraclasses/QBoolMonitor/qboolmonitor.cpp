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

#include "qboolmonitor.h"

QBoolMonitor::QBoolMonitor(QObject *parent) :
    QObject(parent)
{
    lastState = false;
    currentState = false;
}

void QBoolMonitor::compareState(bool input)
{
    currentState = input;
    if(lastState != currentState)
    {
        emit stateChanged();
        emit stateChangedTo(currentState);
        emit stateChangedMessage(message);
        lastState = currentState;
    }
}

void QBoolMonitor::setComparisonState(bool overrideState)
{
    lastState = overrideState;
}

bool QBoolMonitor::getComparisonState()
{
    return lastState;
}

void QBoolMonitor::setMessage(QString m)
{
    message = m;
}

QString QBoolMonitor::getMessage()
{
    return message;
}
