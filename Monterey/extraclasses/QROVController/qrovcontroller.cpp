#include "qrovcontroller.h"
#include <QDebug>

QROVController::QROVController(QObject *parent) :
    QObject(parent)
{
    QMutex mutex;
    mutex.lock();

    numberOfAxes = 0;
    rov = new QROV(numberOfMotors, numberOfRelays, numberOfServos, this);
    joy = new QJoystick();

    mySettings = new QSettings("settings.ini", QSettings::IniFormat);

    txSocket = new QUdpSocket(this);

    captureRx = new UdpCapture(TIBOPORT, ERRORTIMEOUT, this);
    captureTahoe = new UdpCapture(TAHOERXPORT, ERRORTIMEOUT, this);
    capturePi = new UdpCapture(PIRXPORT, PITIMEOUT, this);

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

    foreach(QROVRelay* r, rov->listRelays)
    {
        relayMappings.append(RelayMapping());
    }

    foreach(QROVServo *s, rov->listServos)
    {
        servoMappings.append(ServoMapping());
    }

    joySettings.bilinearEnabled = true;
    joySettings.vectorEnabled = true;

    joySettings.bilinearRatio = 1.5;

    loadSettings();

    connect(captureRx, SIGNAL(packetProcessed(QString)), this, SLOT(processPacket(QString)));
    connect(captureTahoe, SIGNAL(packetProcessed(QString)), this, SLOT(processTahoe(QString)));
    connect(capturePi, SIGNAL(packetProcessed(QString)), this, SLOT(processPi(QString)));

    connect(captureRx, SIGNAL(comChanged(bool)), this, SIGNAL(comTiboChanged(bool)));
    connect(captureTahoe, SIGNAL(comChanged(bool)), this, SIGNAL(comTahoeChanged(bool)));
    connect(capturePi, SIGNAL(comChanged(bool)), this, SIGNAL(comPiChanged(bool)));

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

void QROVController::processPacket(QString packet)
{
    QMutex mutex;
    mutex.lock();
    double version;
    double depth;
    double heading;
    double voltage;
    double sens0;
    double sens1;

    QTextStream rxProcessing(&packet);

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

    emit receivedPacket(packet);
    mutex.unlock();
}

void QROVController::sendPacket()
{
    QMutex mutex;
    mutex.lock();
    QByteArray txDatagram;
    QString txPacket;

    if(rov->motorLayout == QROV::vectorDrive)  //if vector drive
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

    txSocket->writeDatagram(txDatagram.data(), txDatagram.size(), QHostAddress::Broadcast, TOBIPORT);
    emit sentPacket(txPacket);
    mutex.unlock();
}

void QROVController::sendDebug()
{
    //code to send debug packet?
}

void QROVController::processTahoe(QString packet)
{
    QMutex mutex;
    mutex.lock();

    int relay0;
    int relay1;
    int relay2;
    int servo0;
    int servo1;

    //TODO: Make it safe for differening amounts of relays and servos
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
    mutex.unlock();
    emit onTahoeProcessed();    //tell the GUI to update itself
}

void QROVController::sendTahoe()
{
    QMutex mutex;
    mutex.lock();
    QString packet;
    packet.append("1"); //placeholder for comTobi until Tahoe's packet structure is changed
    packet.append(" ");
    packet.append(QString::number((int)captureRx->comStatus()));
    packet.append(" ");
    int isError;
    if(!captureRx->comStatus() || !joyAttached)
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
    txSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, TAHOETXPORT);

    mutex.unlock();
}

void QROVController::processPi(QString packet)
{
    QMutex mutex;
    mutex.lock();
    QHostAddress piAddress = capturePi->senderHostAddress();

    double tempC;
    double uptime;
    double usedMemoryPercentage;
    double usedCpuPercentage;

    QTextStream stream(&packet);
    stream >> tempC >> uptime >> usedMemoryPercentage >> usedCpuPercentage;

    rov->piData->setTempC(tempC);
    rov->piData->setUptimeS((int)uptime);
    rov->piData->setIpAddress(piAddress);
    rov->piData->setUsedMemory(usedMemoryPercentage);
    rov->piData->setUsedCpu(usedCpuPercentage);

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
    return TOBIPORT;
}

int QROVController::getPortTIBO()
{
    return captureRx->socket()->localPort();
}

int QROVController::getPortRpiTibo()
{
    return capturePi->socket()->localPort();
}

void QROVController::loadSettings()
{
    QMutex mutex;
    mutex.lock();
    //TODO: Finish adding settings code and remove it from mainwindow.cpp

    //Load relay names
    for(int i=0; i<rov->listRelays.count(); i++)
    {
        rov->listRelays[i]->setName(mySettings->value("names/relay" + QString::number(i), "relay" + QString::number(i)).toString());
    }

    //Load the units
    rov->sensorDepth->setUnits(mySettings->value("units/depth", "m").toString());
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
    rov->motorLayout = QROV::MotorLayout(mySettings->value("motors/layout", "1").toInt());

    //Bilinear
    joySettings.bilinearEnabled = mySettings->value("bilinear/enabled", "1").toBool();
    joySettings.bilinearRatio = mySettings->value("bilinear/ratio", "1.5").toDouble();

    //Joystick
    joySettings.axisX = mySettings->value("joystick/x", "0").toInt();
    joySettings.axisY = mySettings->value("joystick/y", "0").toInt();
    joySettings.axisZ = mySettings->value("joystick/z", "0").toInt();
    joySettings.axisV = mySettings->value("joystick/v", "0").toInt();
    joySettings.axisL = mySettings->value("joystick/l", "0").toInt();
    joySettings.axisR = mySettings->value("joystick/r", "0").toInt();
    joySettings.deadX = mySettings->value("joystick/deadX", "0").toInt();
    joySettings.deadY = mySettings->value("joystick/deadY", "0").toInt();
    joySettings.deadZ = mySettings->value("joystick/deadZ", "0").toInt();
    joyID = mySettings->value("joystick/id", "0").toInt();

    for(int i=0; i<relayMappings.count(); i++)
    {
        relayMappings[i].button = mySettings->value("joystick/but/r" + QString::number(i)).toInt();
        relayMappings[i].hat = mySettings->value("joystick/hat/r" + QString::number(i)).toInt();
    }

    for(int i=0; i<servoMappings.count(); i++)
    {
        servoMappings[i].hatDown = mySettings->value("joystick/hat/s" + QString::number(i) + "/down").toInt();
        servoMappings[i].hatUp = mySettings->value("joystick/hat/s" + QString::number(i) + "/up").toInt();
        servoMappings[i].buttonDown = mySettings->value("joystick/but/s" + QString::number(i) + "/down").toInt();
        servoMappings[i].buttonUp = mySettings->value("joystick/but/s" + QString::number(i) + "/up").toInt();

    }

    myVectorDrive->initVector(MOTORMIN,MOTORMAX,joySettings.deadX,joySettings.deadY,joySettings.deadZ);

    //Video
    QList<IpVideoFeed*> videoFeeds = rov->getVideoFeeds();
    for(int i = 0;i<videoFeeds.count();i++)
    {
        videoFeeds[i]->setname(mySettings->value("videoFeeds/" + QString::number(i) + "/name", "Main").toString());
        videoFeeds[i]->seturl(mySettings->value("videoFeeds/" + QString::number(i) + "/url", "http://127.0.0.1:8080/javascript_simple.html").toUrl());
        videoFeeds[i]->setAutoGenerate(mySettings->value("videoFeeds/" + QString::number(i) + "/autoGenerate", true).toBool());
    }
    rov->setVideoFeeds(videoFeeds);

    mutex.unlock();
}

void QROVController::saveSettings()
{
    QMutex mutex;
    mutex.lock();
    //Relay Names
    for(int i=0; i<rov->listRelays.count(); i++)
    {
        mySettings->setValue("names/relay" + QString::number(i), rov->listRelays[i]->getName());
    }

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
    mySettings->setValue("motors/layout", rov->motorLayout);

    //Bilinear
    mySettings->setValue("bilinear/enabled", joySettings.bilinearEnabled);
    mySettings->setValue("bilinear/ratio", joySettings.bilinearRatio);

    //Joystick
    mySettings->setValue("joystick/x", joySettings.axisX);
    mySettings->setValue("joystick/y", joySettings.axisY);
    mySettings->setValue("joystick/z", joySettings.axisZ);
    mySettings->setValue("joystick/v", joySettings.axisV);
    mySettings->setValue("joystick/l", joySettings.axisL);
    mySettings->setValue("joystick/r", joySettings.axisR);
    mySettings->setValue("joystick/deadX", joySettings.deadX);
    mySettings->setValue("joystick/deadY", joySettings.deadY);
    mySettings->setValue("joystick/deadZ", joySettings.deadZ);
    mySettings->setValue("joystick/id", joyID);

    for(int i=0; i<relayMappings.count(); i++)
    {
        mySettings->setValue("joystick/but/r" + QString::number(i), relayMappings[i].button);
        mySettings->setValue("joystick/hat/r" + QString::number(i), relayMappings[i].hat);
    }

    for(int i=0; i<servoMappings.count(); i++)
    {
        mySettings->setValue("joystick/hat/s" + QString::number(i) + "/up", servoMappings[i].hatUp);
        mySettings->setValue("joystick/hat/s" + QString::number(i) + "/down", servoMappings[i].hatDown);
        mySettings->setValue("joystick/but/s" + QString::number(i) + "/up", servoMappings[i].buttonUp);
        mySettings->setValue("joystick/but/s" + QString::number(i) + "/down", servoMappings[i].buttonDown);        
    }

    //Video
    for(int i = 0;i<rov->getVideoFeeds().count();i++)
    {
        mySettings->setValue("videoFeeds/" + QString::number(i) + "/name", rov->getVideoFeeds().at(i)->name());
        mySettings->setValue("videoFeeds/" + QString::number(i) + "/url", rov->getVideoFeeds().at(i)->url());
        mySettings->setValue("videoFeeds/" + QString::number(i) + "/autoGenerate", rov->getVideoFeeds().at(i)->autoGenerate());
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
    foreach(RelayMapping r, relayMappings)
    {
        if(buttonID == r.button)
        {
            emit clickRelayButton(r.pushButton);
            qDebug() << "Relay button clicked";
        }
    }
    mutex.unlock();
}

void QROVController::joystickHatClicked(int hatID)
{
    QMutex mutex;
    mutex.lock();
    foreach(RelayMapping r, relayMappings) //used for relays
    {
        if(hatID == r.hat)
        {
            emit clickRelayButton(r.pushButton);
            qDebug() << "Relay hat clicked";
        }
        qDebug() << hatID << r.hat;
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
//Servo values are adjusted here so the user can just press and hold the button/hat and it'll
//keep adjusting its value.  The relays, on the other hand, need to only be pressed momentarily
void QROVController::readMappings()
{
    QMutex mutex;
    mutex.lock();
    for(int h=0; h < joy->hats.count(); h++) //support for multiple hats
    {
        if(joy->hats[h] != 0)   //if hat is not neutral
        {
            for(int i=0;i<servoMappings.count();i++)  //for each servo
            {
                if(joy->hats[h] == servoMappings[i].hatUp) //if increment
                    emit changeServo(i, 1);
                else if(joy->hats[h] == servoMappings[i].hatDown)  //if decrement
                    emit changeServo(i, 0);
            }
        }
    }
    for(int s=0; s < servoMappings.count(); s++) //for each servo
    {
        for(int i=0; i < joy->buttons.count();i++) //for each button
        {
            if(joy->buttons[i] && servoMappings[s].buttonUp == i) //if the up button is pressed
            {
                emit changeServo(s, 1);   //increment
            }
            else if(joy->buttons[i] && servoMappings[s].buttonDown == i) //if the down button is pressed
            {
                emit changeServo(s, 0); //decrement
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
    double bilinearThreshold = 1.0/joySettings.bilinearRatio;
    if(joySettings.bilinearEnabled)
    {
        for(int i=0;i<joy->axis.count();i++)    //for each axis value
        {
            if((bilinearThreshold * -32768) <= joy->axis[i] && joy->axis[i] <= (bilinearThreshold * 32767)) //if the stick is within the range
            {
                joy->axis[i] = (joy->axis[i] / joySettings.bilinearRatio);
            }
            else if((bilinearThreshold * 32767) < joy->axis[i] && joy->axis[i] <= 32767)  //if the stick is in upper section of range
            {
                joy->axis[i] = ((joySettings.bilinearRatio * joy->axis[i]) + ((joySettings.bilinearRatio * -32767)+32767));
            }
            else if(-32768 <= joy->axis[i] && joy->axis[i] < (bilinearThreshold * -32768))    //if the stick is in the lower section of range
            {
                joy->axis[i] = ((joySettings.bilinearRatio * joy->axis[i]) + ((joySettings.bilinearRatio * 32768)-32768));
            }
        }
    }
    for(int i=0;i<numberOfAxes;i++) //record the values of the axes
    {
        joystickAxesValues[i] = joy->axis[i];
    }

    //Execute vector math
    if(rov->motorLayout == QROV::vectorDrive)
    {
        myVectorDrive->vectorMath(joy->axis[joySettings.axisX],joy->axis[joySettings.axisY],joy->axis[joySettings.axisZ],joy->axis[joySettings.axisV],false);

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
        //Pseudo code:
        // Step 1: convert the joystick axis value from [-32768,32727] to [0,65355]
        // Step 2: find the percentage of the stick deflection
        // Step 3: find the value of that percentage of the motor range (add the minimum value to make it within the range)

        const double motorRange = MOTORMAX - MOTORMIN;

        joy->axis[joySettings.axisL] = joy->axis[joySettings.axisL] + 32768;
        double percentL = ((double)joy->axis[joySettings.axisL] / 65355.0);
        rov->listMotors[0]->setValue((int)((percentL * motorRange) + MOTORMIN));

        joy->axis[joySettings.axisR] = joy->axis[joySettings.axisR] + 32768;
        double percentR = ((double)joy->axis[joySettings.axisR] / 65355.0);
        rov->listMotors[1]->setValue((int)((percentR * motorRange) + MOTORMIN));

        joy->axis[joySettings.axisV] = joy->axis[joySettings.axisV] + 32768;
        double percentV = ((double)joy->axis[joySettings.axisV] / 65355.0);
        rov->listMotors[2]->setValue((int)((percentV * motorRange) + MOTORMIN));

    }
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
