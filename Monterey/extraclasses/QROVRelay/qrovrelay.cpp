#include "qrovrelay.h"
#include <QtDebug>

QROVRelay::QROVRelay(QObject *parent) :
    QObject(parent)
{
    enabled = false;
    lastState = enabled;
    name = "Relay";
    button = 0;
    hat = 2;
    pushButton = 0;
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

void QROVRelay::setButton(int b)
{
    button = b;
}

void QROVRelay::setHat(int h)
{
    hat = h;
}

int QROVRelay::getButton()
{
    return button;
}

int QROVRelay::getHat()
{
    return hat;
}

void QROVRelay::setQPushButton(QPushButton *pb)
{
    pushButton = pb;
    qDebug() << pushButton;
}

QPushButton* QROVRelay::getQPushButton()
{
    return pushButton;
}
