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

#include "qrovcontroller.h"
#include <QDebug>

#define numberOfMotors 6
#define numberOfRelays 3
#define numberOfServos 2
#define ERRORTIMEOUT 500
#define MOTORMIN 1000
#define MOTORMAX 2000

QROVController::QROVController(QObject *parent) :
    QObject(parent)
{
    QMutex mutex;
    mutex.lock();
    numberOfAxes = 0;
    rov = new QROV(numberOfMotors, numberOfRelays, numberOfServos, this);
    joy = new QJoystick();
    qDebug() << "Joystick: " << joy;
    mySettings = new QSettings("settings.ini", QSettings::IniFormat);
    rxSocket = new QUdpSocket(this);
    txSocket = new QUdpSocket(this);
    tahoeSocket = new QUdpSocket(this);
    timerTIBO = new QTimer(this);
    timerTOBI = new QTimer(this);
    timerTahoe = new QTimer(this);
    comTIBO = false;
    comTOBI = false;
    comTahoe = false;
    monitorTIBO = new QBoolMonitor(this);
    monitorTIBO->setComparisonState(comTIBO);
    monitorTOBI = new QBoolMonitor(this);
    monitorTOBI->setComparisonState(comTOBI);
    motorLayout = vectorDrive;
    monitorJoystick = new QBoolMonitor(this);
    monitorJoystick->setComparisonState(joyAttached);
    joyID = 0;
    myVectorDrive = new QVectorDrive2(this);
    myVectorDrive->initVector(MOTORMIN, MOTORMAX, 0, 0, 0);

    diveTimer = new DiveTimer(this);
    packetTimer = new QTimer(this);
    packetTimer->setInterval(45);

    initJoysticks();

    rov->setNumMotors(numberOfMotors);
    rov->setNumRelays(numberOfRelays);
    rov->setNumServos(numberOfServos);
    rov->sensorVoltage->setUnits("volts");
    rov->sensorCompass->setUnits("degrees");
    foreach(QROVMotor* m, rov->listMotors)
    {
        m->setMaximum(MOTORMAX);
        m->setMinimum(MOTORMIN);
    }

    bilinearEnabled = true;
    vectorEnabled = true;

    bilinearRatio = 1.5;
    bilinearThreshold = bilinearRatio / 1.0;

    loadSettings();

    timerTIBO->start(ERRORTIMEOUT);
    timerTOBI->start(ERRORTIMEOUT);
    timerTahoe->start(ERRORTIMEOUT);

    tiboPort = 50000;
    tobiPort = 51000;
    tahoeSocket->bind(52000, QUdpSocket::ShareAddress);
    rxSocket->bind(tiboPort, QUdpSocket::ShareAddress);
    connect(rxSocket, SIGNAL(readyRead()), this, SLOT(processPacket()));
    connect(tahoeSocket, SIGNAL(readyRead()), this, SLOT(processTahoe()));
    connect(timerTOBI, SIGNAL(timeout()), this, SLOT(setErrorTOBI()));
    connect(timerTIBO, SIGNAL(timeout()),this, SLOT(setErrorTIBO()));
    connect(timerTahoe, SIGNAL(timeout()), this, SLOT(setErrorTahoe()));
    connect(packetTimer, SIGNAL(timeout()), this, SLOT(motherFunction()));
    connect(joy, SIGNAL(toggleStateChanged(int)), this, SLOT(joystickButtonClicked(int)));
    connect(joy, SIGNAL(hatStateChanged(int)), this, SLOT(joystickHatClicked(int)));

    packetTimer->start();
    mutex.unlock();
    qDebug() << "Controller finished setup!";
}

void QROVController::initJoysticks()
{
    QMutex mutex;
    mutex.lock();
    joysAvail = joy->availableJoysticks();

    if(joysAvail != 0)
    {
        joyAttached = true;
        joy->setJoystick(0);
        numberOfAxes = joy->joystickNumAxes(0);
        joystickAxesValues.clear();
        for(int i=0;i<numberOfAxes;i++)
        {
            joystickAxesValues.append(0);
        }
        qDebug() << "Joystick attached";
    }
    else
    {
        joyAttached = false;
        qWarning() << "No joystick attached!";

    }
    monitorJoystick->compareState(joyAttached);
    mutex.unlock();
    qDebug() << "joysticks setup";
}


void QROVController::rescanJoysticks()
{
    joy->reenumerateDevices();
    getJoysAvail(); //recalculate the number of available joysticks
    initJoysticks();    //re-initialize joysticks
}

QStringList QROVController::getJoystickNames()
{
    QMutex mutex;
    mutex.lock();
    QStringList joystickNames;
    if(joy->availableJoysticks() > 0)
    {
        for(int i=0;i<joy->availableJoysticks();i++)
        {
            joystickNames.append(joy->joystickName(i));
        }
    }
    else
    {
        joystickNames.append("No joystick attached");
    }
    mutex.unlock();
    return joystickNames;
}

int QROVController::getJoystickCurrentHatValue()
{
    if(!joy->hats.isEmpty())
    {
        return joy->hats[0];
    }
    else
    {
        return 0;
    }
}

QList<int> QROVController::getJoystickCurrentButtonValue()
{
    QMutex mutex;
    mutex.lock();
    QList<int> returnValues;
    for(int i=0;i<joy->buttons.count();i++) //for each button
    {
        if(joy->buttons[i] == true) //if button is pressed
        {
            returnValues.append(i);
        }
    }

    if(returnValues.count() == 0)   //if no buttons are pressed
    {
        returnValues.append(-1);    //error value
    }

    mutex.unlock();
    return returnValues;
}

void QROVController::processPacket()
{
    QMutex mutex;
    mutex.lock();
    double version;
    double depth;
    double heading;
    double voltage;
    double sens0;
    double sens1;

    QByteArray rxDatagram;
    QString rxPacket;

    do
    {
        rxDatagram.resize(rxSocket->pendingDatagramSize());
        rxSocket->readDatagram(rxDatagram.data(), rxDatagram.size());
    }
    while(rxSocket->hasPendingDatagrams());
    rxPacket = (tr("\"%1\"").arg(rxDatagram.data()));   //turn datagram into a string
    rxPacket.remove(QChar('"'), Qt::CaseInsensitive);   //remove quotation marks

    QTextStream rxProcessing(&rxPacket);

    rxProcessing >> version >> depth >> heading >> voltage >> sens0 >> sens1;

    rov->setVersion(version);
    rov->sensorDepth->setValue(depth);
    rov->sensorCompass->setValue(heading);
    rov->sensorVoltage->setValue(voltage);
    rov->sensorOther0->setValue(sens0);
    rov->sensorOther1->setValue(sens1);

    if(diveTimer->hasStarted() && rov->sensorDepth->getValue() <= 0)    //if the ROV is at the surface, pause the dive timer
    {
        diveTimer->pause();
    }
    else if(diveTimer->hasStarted() && rov->sensorDepth->getValue() > 0)    //if the ROV is underwater and the dive timer has started BUT MAY BE PAUSED
    {
        diveTimer->resume();
    }
    else if(!diveTimer->hasStarted() && rov->sensorDepth->getValue() > 0)   //if the ROV is underwater and the dive timer hasn't started
    {
        diveTimer->start();
    }

    emit receivedPacket(rxPacket);
    emit noErrorTIBO();
    comTIBO = true;
    monitorTIBO->compareState(comTIBO);
    timerTIBO->start(ERRORTIMEOUT);
    mutex.unlock();
}

void QROVController::sendPacket()
{
    QMutex mutex;
    mutex.lock();
    QByteArray txDatagram;
    QString txPacket;

    if(motorLayout == vectorDrive)  //if vector drive
    {
        foreach(QROVMotor *m, rov->listMotors)
        {
            txPacket.append(QString::number(m->getValue()));
            txPacket.append(" ");
        }
    }
    else    //if tank drive
    {
        for(int i=0;i<3;i++)
        {
            txPacket.append(QString::number(rov->listMotors[i]->getValue()));
            txPacket.append(" ");
        }
    }

    foreach(QROVRelay *r, rov->listRelays)
    {
        if(r->getState() == true)
        {
            txPacket.append(QString::number(1));
        }
        else
        {
            txPacket.append(QString::number(0));
        }

        txPacket.append(" ");
    }

    foreach(QROVServo *s, rov->listServos)
    {
        txPacket.append(QString::number(s->getValue()));
        txPacket.append(" ");
    }

    txDatagram = txPacket.toUtf8();

    txSocket->writeDatagram(txDatagram.data(), txDatagram.size(), QHostAddress::Broadcast, tobiPort);
    emit sentPacket(txPacket);
    emit noErrorTOBI();
    comTOBI = true;
    monitorTOBI->compareState(comTOBI);
    timerTOBI->start(ERRORTIMEOUT);
    mutex.unlock();
}

void QROVController::sendDebug()
{
    //code to send debug packet?
}

void QROVController::processTahoe()
{
    qDebug() << "Received packet from Tahoe!";
    QMutex mutex;
    mutex.lock();
    QByteArray datagram;
    QString packet;

    do
    {
        datagram.resize(tahoeSocket->pendingDatagramSize());
        tahoeSocket->readDatagram(datagram.data(), datagram.size());
    }
    while(tahoeSocket->hasPendingDatagrams());
    packet = (tr("\"%1\"").arg(datagram.data()));   //turn datagram into a string
    packet.remove(QChar('"'), Qt::CaseInsensitive);   //remove quotation marks

    int relay0;
    int relay1;
    int relay2;
    int servo0;
    int servo1;

    QTextStream stream(&packet);
    stream >> relay0 >> relay1 >> relay2 >> servo0 >> servo1;
    if(relay0 == 1)
        rov->listRelays[0]->setState(true);
    else
        rov->listRelays[0]->setState(false);

    if(relay1 == 1)
        rov->listRelays[1]->setState(true);
    else
        rov->listRelays[1]->setState(false);

    if(relay2 == 1)
        rov->listRelays[2]->setState(true);
    else
        rov->listRelays[2]->setState(false);
    rov->listServos[0]->setValue(servo0);
    rov->listServos[1]->setValue(servo1);
    comTahoe = true;
    timerTahoe->start(ERRORTIMEOUT);
    mutex.unlock();
    emit onTahoeProcessed();    //tell the GUI to update itself
}

void QROVController::sendTahoe()
{
    QMutex mutex;
    mutex.lock();
    QString packet;
    packet.append(QString::number((int)comTOBI));
    packet.append(" ");
    packet.append(QString::number((int)comTIBO));
    packet.append(" ");
    int isError;
    if(!comTOBI || !comTIBO || !joyAttached)
        isError = 1;
    else
        isError = 0;
    packet.append(QString::number(isError));
    packet.append(" ");
    packet.append(diveTimeString().remove(QChar(' '), Qt::CaseInsensitive));
    packet.append(" ");
    foreach( QROVServo* s, rov->listServos)
    {
        packet.append(QString::number(s->getValue()));
        packet.append(" ");
    }
    foreach(QROVRelay* r, rov->listRelays)
    {
        if(r->getState() == true)
            packet.append(QString::number(1));
        else
            packet.append(QString::number(0));
        packet.append(" ");
    }
    foreach(QROVRelay* r, rov->listRelays)
    {
        QString specialName = r->getName();
        specialName.remove(QChar(' '), Qt::CaseInsensitive);    //remove spaces
        packet.append(specialName);
        packet.append(" ");
    }
    packet.append(QString::number(rov->sensorDepth->getValue()));
    packet.append(" ");
    packet.append(QString::number(rov->sensorDepth->getMax()));
    packet.append(" ");
    QString depthUnits = rov->sensorDepth->getUnits();
    depthUnits.remove(QChar(' '), Qt::CaseInsensitive); //remove spaces
    packet.append(depthUnits);
    packet.append(" ");
    packet.append(QString::number(rov->sensorCompass->getValue()));
    packet.append(" ");
    packet.append(QString::number(rov->sensorVoltage->getValue()));
    packet.append(" ");
    packet.append(QString::number(rov->sensorOther0->getValue()));
    packet.append(" ");
    packet.append(QString::number(rov->sensorOther1->getValue()));
    packet.append(" ");
    QString newName;
    newName = rov->sensorOther0->getName();
    newName.remove(QChar(' '), Qt::CaseInsensitive);
    packet.append(newName);
    packet.append(" ");
    newName = rov->sensorOther1->getName();
    newName.remove(QChar(' '), Qt::CaseInsensitive);
    packet.append(newName);
    packet.append(" ");
    QString newUnits;
    newUnits = rov->sensorOther0->getUnits();
    newUnits.remove(QChar(' '), Qt::CaseInsensitive);
    packet.append(newUnits);
    packet.append(" ");
    newUnits = rov->sensorOther1->getUnits();
    newUnits.remove(QChar(' '), Qt::CaseInsensitive);
    packet.append(newUnits);
    packet.append(" ");

    QByteArray datagram = packet.toUtf8();
    txSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, 53000);

    mutex.unlock();
}

void QROVController::noJoystick()
{
    for(int i=0;i<rov->listMotors.count();i++)
    {
        rov->listMotors[i]->setValue(1500); //set everything neutral
    }
}

int QROVController::getPortTOBI()
{
    return tobiPort;
}

int QROVController::getPortTIBO()
{
    return tiboPort;
}

void QROVController::loadSettings()
{
    QMutex mutex;
    mutex.lock();
    //TODO: Finish adding settings code and remove it from mainwindow.cpp

    //Load relay names
    rov->listRelays[0]->setName(mySettings->value("names/relay0", "relay0").toString());
    rov->listRelays[1]->setName(mySettings->value("names/relay1", "relay1").toString());
    rov->listRelays[2]->setName(mySettings->value("names/relay2", "relay2").toString());

    //Load the units
    rov->sensorDepth->setUnits(mySettings->value("units/depth", "meters").toString());
    rov->sensorOther0->setUnits(mySettings->value("units/sensor0", "units").toString());
    rov->sensorOther1->setUnits(mySettings->value("units/sensor1", "units").toString());

    //Load the names
    rov->sensorOther0->setName(mySettings->value("names/sensor0", "Sensor0").toString());
    rov->sensorOther1->setName(mySettings->value("names/sensor1", "Sensor1").toString());

    //Load thresholds
    rov->sensorDepth->setMax(mySettings->value("thresholds/depth", "10.0").toDouble());
    rov->sensorDepth->setThreshold(rov->sensorDepth->getMax());
    rov->sensorVoltage->setThreshold(mySettings->value("thresholds/voltage", "9").toDouble());

    //Load motor settings
    setMotorLayout(mySettings->value("motors/layout", "0").toInt());

    //Bilinear
    bilinearEnabled = mySettings->value("bilinear/enabled", "1").toBool();
    bilinearRatio = mySettings->value("bilinear/ratio", "1.5").toDouble();
    bilinearThreshold = bilinearRatio/1.0;

    //Joystick
    axisX = mySettings->value("joystick/x", "0").toInt();
    axisY = mySettings->value("joystick/y", "0").toInt();
    axisZ = mySettings->value("joystick/z", "0").toInt();
    axisV = mySettings->value("joystick/v", "0").toInt();
    axisL = mySettings->value("joystick/l", "0").toInt();
    axisR = mySettings->value("joystick/r", "0").toInt();
    xDead = mySettings->value("joystick/deadX", "0").toInt();
    yDead = mySettings->value("joystick/deadY", "0").toInt();
    zDead = mySettings->value("joystick/deadZ", "0").toInt();
    joyID = mySettings->value("joystick/id", "0").toInt();
    rov->listRelays[0]->setButton(mySettings->value("joystick/but/r0").toInt());
    rov->listRelays[1]->setButton(mySettings->value("joystick/but/r1").toInt());
    rov->listRelays[2]->setButton(mySettings->value("joystick/but/r2").toInt());
    rov->listRelays[0]->setHat(mySettings->value("joystick/hat/r0").toInt());
    rov->listRelays[1]->setHat(mySettings->value("joystick/hat/r1").toInt());
    rov->listRelays[2]->setHat(mySettings->value("joystick/hat/r2").toInt());
    rov->listServos[0]->setHatDown(mySettings->value("joystick/hat/s0/down").toInt());
    rov->listServos[0]->setHatUp(mySettings->value("joystick/hat/s0/up").toInt());
    rov->listServos[1]->setHatDown(mySettings->value("joystick/hat/s1/down").toInt());
    rov->listServos[1]->setHatUp(mySettings->value("joystick/hat/s1/up").toInt());
    rov->listServos[0]->setButtonDown(mySettings->value("joystick/but/s0/down").toInt());
    rov->listServos[0]->setButtonUp(mySettings->value("joystick/but/s0/up").toInt());
    rov->listServos[1]->setButtonDown(mySettings->value("joystick/but/s1/down").toInt());
    rov->listServos[1]->setButtonUp(mySettings->value("joystick/but/s1/up").toInt());

    myVectorDrive->initVector(MOTORMIN,MOTORMAX,xDead,yDead,zDead);

    //Video
    QList<IpVideoFeed*> videoFeeds = rov->getVideoFeeds();
    for(int i = 0;i<videoFeeds.count();i++)
    {
        //mySettings->setValue("videoFeeds/" + i + "/name", rov->getVideoFeeds().at(i)->name());
        //mySettings->setValue("videoFeeds/" + i + "/url", rov->getVideoFeeds().at(i)->url());
        videoFeeds[i]->setname(mySettings->value("videoFeeds/" + QString::number(i) + "/name", "Main").toString());
        videoFeeds[i]->seturl(mySettings->value("videoFeeds/" + QString::number(i) + "/url", "http://127.0.0.1:8080").toUrl());
    }

    mutex.unlock();
}

void QROVController::saveSettings()
{
    QMutex mutex;
    mutex.lock();
    //Relay Names
    mySettings->setValue("names/relay0", rov->listRelays[0]->getName());
    mySettings->setValue("names/relay1", rov->listRelays[1]->getName());
    mySettings->setValue("names/relay2", rov->listRelays[2]->getName());

    //Units
    mySettings->setValue("units/depth", rov->sensorDepth->getUnits());
    mySettings->setValue("units/sensor0", rov->sensorOther0->getUnits());
    mySettings->setValue("units/sensor1", rov->sensorOther1->getUnits());

    //Names
    mySettings->setValue("names/sensor0", rov->sensorOther0->getName());
    mySettings->setValue("names/sensor1", rov->sensorOther1->getName());

    //Thresholds
    mySettings->setValue("thresholds/depth", rov->sensorDepth->getThreshold());
    mySettings->setValue("thresholds/voltage", rov->sensorVoltage->getThreshold());

    //Motors
    mySettings->setValue("motors/layout", motorLayout);

    //Bilinear
    mySettings->setValue("bilinear/enabled", bilinearEnabled);
    mySettings->setValue("bilinear/ratio", bilinearRatio);

    //Joystick
    mySettings->setValue("joystick/x", axisX);
    mySettings->setValue("joystick/y", axisY);
    mySettings->setValue("joystick/z", axisZ);
    mySettings->setValue("joystick/v", axisV);
    mySettings->setValue("joystick/l", axisL);
    mySettings->setValue("joystick/r", axisR);
    mySettings->setValue("joystick/deadX", xDead);
    mySettings->setValue("joystick/deadY", yDead);
    mySettings->setValue("joystick/deadZ", zDead);
    mySettings->setValue("joystick/id", joyID);
    mySettings->setValue("joystick/but/r0", rov->listRelays[0]->getButton());
    mySettings->setValue("joystick/but/r1", rov->listRelays[1]->getButton());
    mySettings->setValue("joystick/but/r2", rov->listRelays[2]->getButton());
    mySettings->setValue("joystick/hat/r0", rov->listRelays[0]->getHat());
    mySettings->setValue("joystick/hat/r1", rov->listRelays[1]->getHat());
    mySettings->setValue("joystick/hat/r2", rov->listRelays[2]->getHat());
    mySettings->setValue("joystick/hat/s0/up", rov->listServos[0]->getHatUp());
    mySettings->setValue("joystick/hat/s0/down", rov->listServos[0]->getHatDown());
    mySettings->setValue("joystick/hat/s1/up", rov->listServos[1]->getHatUp());
    mySettings->setValue("joystick/hat/s1/down", rov->listServos[1]->getHatDown());
    mySettings->setValue("joystick/but/s0/up", rov->listServos[0]->getButtonUp());
    mySettings->setValue("joystick/but/s0/down", rov->listServos[0]->getButtonDown());
    mySettings->setValue("joystick/but/s1/up", rov->listServos[1]->getButtonUp());
    mySettings->setValue("joystick/but/s1/down", rov->listServos[1]->getButtonDown());

    //Video
    for(int i = 0;i<rov->getVideoFeeds().count();i++)
    {
        mySettings->setValue("videoFeeds/" + QString::number(i) + "/name", rov->getVideoFeeds().at(i)->name());
        mySettings->setValue("videoFeeds/" + QString::number(i) + "/url", rov->getVideoFeeds().at(i)->url());
    }

    mutex.unlock();
    emit savedSettings("Settings saved");
}

void QROVController::joystickButtonClicked(int buttonID)
{
    //Check each relay to see if it's joystick button was clicked,
    //if so, then click the QPushButton corresponding to each relay
    QMutex mutex;
    mutex.lock();
    foreach(QROVRelay * r, rov->listRelays)
    {
        if(buttonID == r->getButton())
        {
            emit clickRelayButton(r->getQPushButton());
            qDebug() << "Relay button clicked";
        }
    }
    mutex.unlock();
}

void QROVController::joystickHatClicked(int hatID)
{
    QMutex mutex;
    mutex.lock();
    qDebug() << "In joystickHatClicked(int hatID) function";
    foreach(QROVRelay * r, rov->listRelays) //used for relays
    {
        if(hatID == r->getHat())
        {
            emit clickRelayButton(r->getQPushButton());
            qDebug() << "Relay hat clicked";
        }
        qDebug() << hatID << r->getHat();
    }
    mutex.unlock();
}

void QROVController::motherFunction()
{
    QMutex mutex;
    mutex.lock();
    if(joysAvail !=0 )
    {
        updateJoystickData();
        readMappings();
    }
    else
    {
        noJoystick();
    }
    mutex.unlock();
    sendPacket();
    sendDebug();
    sendTahoe();
    emit onMotherFunctionCompleted();
}

int QROVController::mapInt(int input, int inMin, int inMax, int outMin, int outMax)
{
    int output = (input - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    return output;
}

//This is for the non-event based mappings.  To read the event based mappings, look to the
//joystickButtonClicked and joystickHatClicked functions
void QROVController::readMappings()
{
    QMutex mutex;
    mutex.lock();
    for(int h=0; h< joy->hats.count(); h++) //support for multiple hats
    {
        if(joy->hats[h] != 0)   //if hat is not neutral
        {
            for(int i=0;i<rov->listServos.count();i++)  //for each servo
            {
                if(joy->hats[h] == rov->listServos[i]->getHatUp()) //if increment
                    emit changeServo(i, 1);
                else if(joy->hats[h] == rov->listServos[i]->getHatDown())  //if decrement
                    emit changeServo(i, 0);
            }
        }
    }
    for(int s=0; s<rov->listServos.count(); s++) //for each servo
    {
        for(int i=0;i<joy->buttons.count();i++) //for each button
        {
            if(joy->buttons[i] == true && rov->listServos[s]->getButtonUp() == i) //if the up button is pressed
            {
                //increment
                qDebug() << "Button ID: " << i << "Servo Increment";
                emit changeServo(s, 1);
            }
            else if(joy->buttons[i] == true && rov->listServos[s]->getButtonDown() == i) //if the down button is pressed
            {
                //decrement
                qDebug() << "Button ID: " << i << "Servo Decrement";
                emit changeServo(s, 0);
            }
            else
            {
                //do nothing
            }
        }
    }
    mutex.unlock();
}


void QROVController::updateJoystickData()
{
    QMutex mutex;
    mutex.lock();
    joy->getdata(); //read the joystick (within QJoystick)

    //Bilinear reading code
    bilinearThreshold = 1.0/bilinearRatio;
    if(bilinearEnabled)
    {
        for(int i=0;i<joy->axis.count();i++)    //for each axis value
        {
            if((bilinearThreshold * -32768) <= joy->axis[i] && joy->axis[i] <= (bilinearThreshold * 32767)) //if the stick is within the range
            {
                joy->axis[i] = (joy->axis[i] / bilinearRatio);
            }
            else if((bilinearThreshold * 32767) < joy->axis[i] && joy->axis[i] <= 32767)  //if the stick is in upper section of range
            {
                joy->axis[i] = ((bilinearRatio * joy->axis[i]) + ((bilinearRatio * -32767)+32767));
            }
            else if(-32768 <= joy->axis[i] && joy->axis[i] < (bilinearThreshold * -32768))    //if the stick is in the lower section of range
            {
                joy->axis[i] = ((bilinearRatio * joy->axis[i]) + ((bilinearRatio * 32768)-32768));
            }
        }
    }
    for(int i=0;i<numberOfAxes;i++) //record the values of the axes
    {
        joystickAxesValues[i] = joy->axis[i];
    }

    //Execute vector math
    if(motorLayout == vectorDrive)
    {
        myVectorDrive->vectorMath(joy->axis[axisX],joy->axis[axisY],joy->axis[axisZ],joy->axis[axisV],false);

        if(rov->getNumMotors() == 6)
        {
            for(int i=0;i<rov->getNumMotors();i++)    //retrieve vector values
            {
                rov->listMotors[i]->setValue(myVectorDrive->getVectorValue(i));
            }
        }

    }
    else    //if tank drive
    {
        joy->axis[axisL] = joy->axis[axisL] + 32768;
        double percentL = ((double)joy->axis[axisL] / 65355.0);   //find the percent of the joystick movement
        rov->listMotors[0]->setValue((int)((percentL * 1000.0) + 1000.0));    //convert it to a value in the 1000->2000 range

        joy->axis[axisR] = joy->axis[axisR] + 32768;
        double percentR = ((double)joy->axis[axisR] / 65355.0);   //find the percent of the joystick movement
        rov->listMotors[1]->setValue((int)((percentR * 1000.0) + 1000.0));    //convert it to a value in the 1000->2000 range

        joy->axis[axisV] = joy->axis[axisV] + 32768;
        double percentV = ((double)joy->axis[axisV] / 65355.0);   //find the percent of the joystick movement
        rov->listMotors[2]->setValue((int)((percentV * 1000.0) + 1000.0));    //convert it to a value in the 1000->2000 range

    }
    mutex.unlock();
}

void QROVController::setErrorTOBI()
{
    QMutex mutex;
    mutex.lock();
    comTOBI = false;
    monitorTOBI->compareState(comTOBI);
    mutex.unlock();
    emit errorTOBI();
}

void QROVController::setErrorTIBO()
{
    QMutex mutex;
    mutex.lock();
    comTIBO = false;
    monitorTIBO->compareState(comTIBO);
    mutex.unlock();
    emit errorTIBO();
}

void QROVController::setErrorTahoe()
{
    QMutex mutex;
    mutex.lock();
    comTahoe = false;
    mutex.unlock();
}

void QROVController::diveTimeReset()
{
    diveTimer->reset();
    emit appendToActivityMonitor("Dive timer reset");
}

QString QROVController::diveTimeString()
{
    return diveTimer->diveTimeString();
}
