#include "divetimer.h"

DiveTimer::DiveTimer(QObject *parent) :
    QObject(parent)
{
    timer = new QTimer(this);
    secondsElapsed = 0;
    paused = false;
    started = false;

    connect(timer, &QTimer::timeout, this, &DiveTimer::onTick);
    timer->start(1000);
}

void DiveTimer::start()
{
    reset();
    paused = false;
    started = true;
}

void DiveTimer::pause()
{
    paused = true;
}

void DiveTimer::resume()
{
    paused = false;
}

void DiveTimer::reset()
{
    pause();
    secondsElapsed = 0;
    started = false;
}

QString DiveTimer::diveTimeString()
{
    QString diveTimeString;
    unsigned int hours = secondsElapsed / (60 * 60);    //convert seconds to hours
    unsigned int minutes = (secondsElapsed % (60 * 60)) / (60);  //convert to minutes
    unsigned int seconds = ((secondsElapsed % (60 * 60)) % (60));    //convert to seconds

    diveTimeString.append(QString::number(hours).rightJustified(2, '0'));   //add leading zeros
    diveTimeString.append(":");
    diveTimeString.append(QString::number(minutes).rightJustified(2,'0'));  //add leading zeros
    diveTimeString.append(":");
    diveTimeString.append(QString::number(seconds).rightJustified(2, '0')); //add leading zeros

    return diveTimeString;
}

bool DiveTimer::hasStarted()
{
    return started;
}

void DiveTimer::onTick()
{
    if(!paused && started)
        secondsElapsed++;
}
