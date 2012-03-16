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
