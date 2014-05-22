#include "qrovrelay.h"
#include <QtDebug>

QROVRelay::QROVRelay(QObject *parent) :
    QObject(parent)
{
    enabled = false;
    lastState = enabled;
    name = "Relay";
}

QString QROVRelay::getName()
{
    return name;
}

void QROVRelay::setName(QString n)
{
    name = n;
}

bool QROVRelay::getState()
{
    return enabled;
}

void QROVRelay::setState(bool s)
{
    enabled = s;
    if(lastState != enabled)
    {
        emit toggled(enabled);
        lastState = enabled;
    }
}
