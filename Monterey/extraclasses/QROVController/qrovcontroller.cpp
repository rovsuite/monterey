#include "qrovcontroller.h"

#define numberOfMotors 6
#define numberOfRelays 3
#define numberOfServos 2
#define ERRORTIMEOUT 500

QROVController::QROVController(QObject *parent) :
    QObject(parent)
{
    rov = new QROV(this);
    joy = new QJoystick();
    //mySettings = new QSettings("rovsuite", "monterey", this);
    mySettings = new QSettings("settings.ini", QSettings::IniFormat);
    rxSocket = new QUdpSocket(this);
    txSocket = new QUdpSocket(this);
    timerTIBO = new QTimer(this);
    timerTOBI = new QTimer(this);
    comTIBO = false;
    comTOBI = false;
    monitorTIBO = new QBoolMonitor(this);
    monitorTIBO->setComparisonState(comTIBO);
    monitorTOBI = new QBoolMonitor(this);
    monitorTOBI->setComparisonState(comTOBI);
    motorLayout = vectorDrive;
    monitorJoystick = new QBoolMonitor(this);
    monitorJoystick->setComparisonState(joyAttached);

    initJoysticks();

    rov->sensorDepth->setUnits("meters");
    rov->sensorCompass->setUnits("degrees");
    rov->sensorVoltage->setUnits("volts");
    rov->sensorOther0->setUnits("units");
    rov->sensorOther1->setUnits("units");

    rov->setNumMotors(numberOfMotors);
    rov->setNumRelays(numberOfRelays);
    rov->setNumServos(numberOfServos);

    bilinearEnabled = true;
    vectorEnabled = true;

    bilinearThreshold = 0.6;
    bilinearRatio = 1.5;

    loadSettings();

    timerTIBO->start(ERRORTIMEOUT);
    timerTOBI->start(ERRORTIMEOUT);

    tiboPort = 50000;
    tobiPort = 51000;
    rxSocket->bind(tiboPort, QUdpSocket::ShareAddress);
    connect(rxSocket, SIGNAL(readyRead()), this, SLOT(processPacket()));
    connect(timerTOBI, SIGNAL(timeout()), this, SLOT(setErrorTOBI()));
    connect(timerTIBO, SIGNAL(timeout()),this, SLOT(setErrorTIBO()));
}

void QROVController::initJoysticks()
{
    joysAvail = joy->availableJoysticks();

    if(joysAvail != 0)
    {
        joyAttached = true;
        //joy->setJoystick(0);
    }
    else
    {
        joyAttached = false;
    }
    monitorJoystick->compareState(joyAttached);
}


void QROVController::rescanJoysticks()
{
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    SDL_Init(SDL_INIT_JOYSTICK);
    getJoysAvail(); //recalculate the number of available joysticks
    initJoysticks();    //re-initialize joysticks
}

QStringList QROVController::getJoystickNames()
{
    QStringList joystickNames;
    for(int i=0;i<joy->availableJoysticks();i++)
    {
        joystickNames.append(joy->joystickName(i));
    }
    return joystickNames;
}

void QROVController::processPacket()
{
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

    emit receivedPacket(rxPacket);
    emit noErrorTIBO();
    comTIBO = true;
    monitorTIBO->compareState(comTIBO);
    timerTIBO->start(ERRORTIMEOUT);
}

void QROVController::sendPacket()
{
    QByteArray txDatagram;
    QString txPacket;

    foreach(QROVMotor *m, rov->listMotors)
    {
        txPacket.append(QString::number(m->getValue()));
        txPacket.append(" ");
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

}

void QROVController::sendDebug()
{
    // TODO: Add in code to send debug packet
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

double QROVController::getMaxDepth()
{
    // WARNING: Possibly remove getMaxDepth() function?
    return rov->sensorDepth->getMax();
}

void QROVController::loadSettings()
{
    //TODO: Add settings code (including save settings)
    //TDOD: Finish adding settings code and remove it from mainwindow.cpp

    //Load relay names
    rov->listRelays[0]->setName(mySettings->value("names/relay0").toString());
    rov->listRelays[1]->setName(mySettings->value("names/relay1").toString());
    rov->listRelays[2]->setName(mySettings->value("names/relay2").toString());

    //Load the units
    rov->sensorDepth->setUnits(mySettings->value("units/depth", "meters").toString());
    rov->sensorOther0->setUnits(mySettings->value("units/sensor0", "units").toString());
    rov->sensorOther1->setUnits(mySettings->value("units/sensor1", "units").toString());

    //Load thresholds
    rov->sensorDepth->setMax(mySettings->value("thresholds/depth", "10.0").toDouble());
    rov->sensorDepth->setThreshold(rov->sensorDepth->getMax());
    rov->sensorVoltage->setThreshold(mySettings->value("thresholds/voltage", "9").toDouble());

    //Load motor settings
    setMotorLayout(mySettings->value("motors/layout", "0").toInt());

    //Bilinear
    bilinearEnabled = mySettings->value("bilinear/enabled", "1").toBool();
    bilinearRatio = mySettings->value("bilinear/ratio", "0.0").toDouble();
    bilinearThreshold = mySettings->value("bilinear/thresold", "0.0").toDouble();
}

void QROVController::saveSettings()
{
    //Relay Names
    mySettings->setValue("names/relay0", rov->listRelays[0]->getName());
    mySettings->setValue("names/relay1", rov->listRelays[1]->getName());
    mySettings->setValue("names/relay2", rov->listRelays[2]->getName());

    //Units
    mySettings->setValue("units/depth", rov->sensorDepth->getUnits());
    mySettings->setValue("units/sensor0", rov->sensorOther0->getUnits());
    mySettings->setValue("units/sensor1", rov->sensorOther1->getUnits());

    //Thresholds
    mySettings->setValue("thresholds/depth", rov->sensorDepth->getThreshold());
    mySettings->setValue("thresholds/voltage", rov->sensorVoltage->getThreshold());

    //Motors
    mySettings->setValue("motors/layout", motorLayout);

    //Bilinear
    mySettings->setValue("bilinear/enabled", bilinearEnabled);
    mySettings->setValue("bilinear/ratio", bilinearRatio);
    mySettings->setValue("bilinear/threshold", bilinearThreshold);

    emit savedSettings("Settings saved");
}

void QROVController::motherFunction()
{
    if(joysAvail !=0 )
    {
        // TODO: Joystick reading code
        //updateJoystickData();
    }
    else
    {
        noJoystick();
    }
    sendPacket();
    sendDebug();
}

int QROVController::mapInt(int input, int inMin, int inMax, int outMin, int outMax)
{
    int output = (input - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    return output;
}

void QROVController::readMappings()
{
    // TODO: Add in code to load up joystick mappings
}


void QROVController::updateJoystickData()
{
    // TODO: Add in tank drive stuff
    joy->getdata(); //read the joystick (within QJoystick)

    //Bilinear reading code
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

    //Execute vector math
    if(vectorEnabled)
    {
        // TODO: Remove reading of QSettings and move that to the MainWindow class?

        myVectorDrive->vectorMath(joy->axis[mySettings->value("axes/vector/x").toInt()],joy->axis[mySettings->value("axes/vector/y").toInt()],joy->axis[mySettings->value("axes/vector/z").toInt()],joy->axis[mySettings->value("axes/vector/v").toInt()],false);

        if(rov->getNumMotors() == 6)
        {
            for(int i=0;i<rov->getNumMotors();i++)    //retrieve vector values
            {
                rov->listMotors[i]->setValue(myVectorDrive->getVectorValue(i));
            }
        }

    }

    // TODO: Add hat and button reading
}

void QROVController::setErrorTOBI()
{
    comTOBI = false;
    monitorTOBI->compareState(comTOBI);
    emit errorTOBI();
}

void QROVController::setErrorTIBO()
{
    comTIBO = false;
    monitorTIBO->compareState(comTIBO);
    emit errorTIBO();
}
