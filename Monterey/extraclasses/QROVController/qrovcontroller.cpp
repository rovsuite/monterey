#include "qrovcontroller.h"
#include "../../extraclasses/ConfigParser/configparser.h"
#include <QDebug>
#include <QJsonObject>
#include <QDir>
#include "../../extraclasses/QROV/qrov.h"
#include "../../extraclasses/QJoystick/qjoystick.h"

QROVController::QROVController(bool& enteredGoodState, QString& statusMessage, QObject *parent) :
    QObject(parent)
{
    QMutex mutex;
    mutex.lock();

    enteredGoodState = true;    //default to success

    //Parse the ROV configuration file
    QDir dir;
    QString filePath = dir.absoluteFilePath("rov.json");
    ConfigParser rovConfigParser(filePath, this);
    if(!rovConfigParser.parseRov(mRov))    //try to read the user-specified config
    {
        qWarning() << "Could not parse ROV configuration file.  Loading defaults.";
        //If loading the user specified ROV file failed, then try loading a default file
        rovConfigParser.setFile(":/default/rov.json");
        statusMessage = "Could not load ROV config. Loading defaults.";
        if(!rovConfigParser.parseRov(mRov))
        {

            qCritical() << "Could not parse ROV configuration file, QUITTING";
            statusMessage = "Could not load defaults, QUITTING.";
            enteredGoodState = false;
        }
        else
        {
            statusMessage = "Loaded default ROV configuration.";
        }
    }
    else
    {
        statusMessage = QString("Loaded ROV configuration: %1").arg(filePath);
    }

    joy = new QJoystick();

    mySettings = new QSettings("settings.ini", QSettings::IniFormat);

    txSocket = new QUdpSocket(this);

    captureRx = new UdpCapture(TIBOPORT, ERRORTIMEOUT, this);
    capturePi = new UdpCapture(PIRXPORT, PITIMEOUT, this);

    monitorJoystick = new QBoolMonitor(this);
    monitorJoystick->setComparisonState(joy->joystickAttached());

    myVectorDrive = new QVectorDrive2(this);
    myVectorDrive->initVector(MOTORMIN, MOTORMAX);

    diveTimer = new DiveTimer(this);
    packetTimer = new QTimer(this);
    packetTimer->setInterval(45);

    initJoysticks();

    foreach(QROVRelay r, mRov.relays)
    {
        relayMappings.append(RelayMapping());
    }

    foreach(QROVServo s, mRov.servos)
    {
        servoMappings.append(ServoMapping());
    }

    joySettings.vectorEnabled = true;

    loadSettings();

    connect(captureRx, SIGNAL(packetProcessed(QString)), this, SLOT(processPacket(QString)));
    connect(capturePi, SIGNAL(packetProcessed(QString)), this, SLOT(processPi(QString)));

    connect(captureRx, SIGNAL(comChanged(bool)), this, SIGNAL(comTiboChanged(bool)));
    connect(capturePi, SIGNAL(comChanged(bool)), this, SIGNAL(comPiChanged(bool)));

    connect(packetTimer, SIGNAL(timeout()), this, SLOT(motherFunction()));
    connect(joy, SIGNAL(axesUpdated(QList<int>)), this, SLOT(onAxesUpdated(QList<int>)));
    connect(joy, SIGNAL(buttonPressed(int)), this, SLOT(onButtonPressed(int)));
    connect(joy, SIGNAL(buttonReleased(int)), this, SLOT(onButtonReleased(int)));
    connect(joy, SIGNAL(buttonToggled(int,bool)), this, SLOT(onButtonToggled(int,bool)));
    connect(joy, SIGNAL(hatPressed(int,int)), this, SLOT(onHatPressed(int,int)));
    connect(joy, SIGNAL(hatReleased(int,int)), this, SLOT(onHatReleased(int,int)));
    connect(joy, SIGNAL(hatToggled(int,int,bool)), this, SLOT(onHatToggled(int,int,bool)));

    setValidity(enteredGoodState);
    packetTimer->start();
    mutex.unlock();
    qDebug() << "Controller finished setup!";
}

QROVController::~QROVController()
{
    saveSettings();

    delete monitorJoystick;
    delete mySettings;
    delete captureRx;
    delete capturePi;
    delete txSocket;
    delete joy;
    delete diveTimer;
    delete packetTimer;
    delete myVectorDrive;
}

bool QROVController::saveRovLog(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QString message;
    if(file.isOpen())
    {
        message = "Opened log file: %1";
        message = message.arg(filename);
        emit appendToActivityMonitor(message, MsgType::Info);
        QJsonArray rovArray;
        while(!rovHistory.isEmpty())
        {
            rovArray.append(getRovAsJsonObject(rovHistory.front()));
            rovHistory.pop_front();
        }

        QJsonDocument doc(rovArray);
        QTextStream out(&file);

        out << doc.toJson();
        file.close();

        if(out.status() != QTextStream::WriteFailed)
        {
            message = "Saved log file: %1";
            message = message.arg(filename);
            emit appendToActivityMonitor(message, MsgType::Good);
            return true;
        }
    }

    message = "Failed to save log: %1\nError: %2";
    message = message.arg(filename, file.errorString());

    //If the saving failed
    emit appendToActivityMonitor(message, MsgType::Bad);
    return false;
}

void QROVController::enableLogging(bool enable)
{
    mLoggingEnabled = enable;
    emit appendToActivityMonitor(QString("Dive logging ") + (enable ? "enabled" : "disabled"), (enable ? MsgType::Good : MsgType::Bad));
}

void QROVController::clearLog()
{
    rovHistory.clear();
    emit appendToActivityMonitor("Dive log cleared", MsgType::Warn);
}

void QROVController::initJoysticks()
{
    QMutex mutex;
    mutex.lock();

    if(0 < joy->numJoysticks())
    {
        joy->currentJoystick(0);

        qDebug() << "Joystick attached";
    }
    else
    {
        qWarning() << "No joystick attached!";

    }
    monitorJoystick->compareState(joy->joystickAttached());
    mutex.unlock();
}


void QROVController::rescanJoysticks()
{
    joy->reenumerateDevices();
    initJoysticks();    //re-initialize joysticks
}

QStringList QROVController::getJoystickNames()
{
    QMutex mutex;
    mutex.lock();
    QStringList joystickNames;
    if(joy->numJoysticks() > 0)
    {
        int curJoystick = joy->currentJoystick();
        for(int i=0;i<joy->numJoysticks();i++)
        {
            joy->currentJoystick(i);
            joystickNames.append(joy->name());
        }
        joy->currentJoystick(curJoystick);
    }
    else
    {
        joystickNames.append("No joystick attached");
    }
    mutex.unlock();
    return joystickNames;
}

void QROVController::onButtonPressed(int button)
{
    //Leave empty for now
}

void QROVController::onButtonReleased(int button)
{
    //Leave empty for now
}

void QROVController::onButtonToggled(int button, bool state)
{
    //Check each relay to see if it's joystick button was clicked,
    //if so, then click the QPushButton corresponding to each relay
    QMutex mutex;
    mutex.lock();
    foreach(RelayMapping r, relayMappings)
    {
        if(button == r.button)
        {
            emit clickRelayButton(r.pushButton);
        }
    }
    mutex.unlock();
}

void QROVController::onHatPressed(int hat, int dir)
{
    //Leave empty for now
}

void QROVController::onHatReleased(int hat, int dir)
{
    //Leave empty for now
}

void QROVController::onHatToggled(int hat, int dir, bool state)
{
    QMutex mutex;
    mutex.lock();
    if(0 != dir)
    {
        foreach(RelayMapping r, relayMappings) //used for relays
        {
            if(hat == r.hat && dir == r.dir)
            {
                emit clickRelayButton(r.pushButton);
            }
        }
    }
    mutex.unlock();
}

void QROVController::onAxesUpdated(const QList<int>& values)
{
    QMutex mutex;
    mutex.lock();

    const int numValues = values.count();
    Q_ASSERT(joySettings.axisX < numValues);
    Q_ASSERT(joySettings.axisY < numValues);
    Q_ASSERT(joySettings.axisZ < numValues);
    Q_ASSERT(joySettings.axisV < numValues);
    Q_ASSERT(joySettings.axisL < numValues);
    Q_ASSERT(joySettings.axisR < numValues);

    //Execute vector math
    if(mRov.motorLayout == vectorDrive)
    {
        Q_ASSERT(mRov.motors.count() == 6);
        myVectorDrive->vectorMath(values[joySettings.axisX],
               values[joySettings.axisY],
               values[joySettings.axisZ],
               values[joySettings.axisV],
                false);

        for(int i=0;i<mRov.motors.count();i++)    //retrieve vector values
        {
            mRov.motors[i].value = myVectorDrive->getVectorValue(i);
        }
    }
    else    //if tank drive
    {
        //Pseudo code:
        // Step 1: convert the joystick axis value from [-32768,32727] to [0,65355]
        // Step 2: find the percentage of the stick deflection
        // Step 3: find the value of that percentage of the motor range (add the minimum value to make it within the range)

        const double motorRange = MOTORMAX - MOTORMIN;

        double percentL = ((double)(values[joySettings.axisL] + 32768) / 65355.0);
        mRov.motors[0].value = (int)((percentL * motorRange) + MOTORMIN);

        double percentR = ((double)(values[joySettings.axisR] + 32768) / 65355.0);
        mRov.motors[1].value = (int)((percentR * motorRange) + MOTORMIN);

        double percentV = ((double)(values[joySettings.axisV] + 32768) / 65355.0);
        mRov.motors[2].value = (int)((percentV * motorRange) + MOTORMIN);
    }

    mutex.unlock();
}

void QROVController::processPacket(QString packet)
{
    QMutex mutex;
    mutex.lock();

    //Parse the packet
    QTextStream rxProcessing(&packet);
    double depth = 0;

    rxProcessing >> mRov.version;
    foreach(QROVSensor sensor, mRov.sensors)
    {
        rxProcessing >> sensor.value;

        //Temporarily store the depth value if possible
        if(sensor.name.toLower() == "depth")
        {
            depth = sensor.value;
        }
    }

    //Automatically control the dive timer based on the cached depth value
    if(diveTimer->hasStarted() && depth <= 0)    //if the ROV is at the surface, pause the dive timer
    {
        diveTimer->pause();
    }
    else if(diveTimer->hasStarted() && depth > 0)    //if the ROV is underwater and the dive timer has started BUT MAY BE PAUSED
    {
        diveTimer->resume();
    }
    else if(!diveTimer->hasStarted() && depth > 0)   //if the ROV is underwater and the dive timer hasn't started
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

    for(int i=0; i<mRov.motors.count(); i++)
    {
        txPacket.append(QString::number(mRov.motors[i].value));
        txPacket.append(" ");
    }

    foreach(QROVRelay r, mRov.relays)
    {
        if(r.enabled == true)
        {
            txPacket.append(QString::number(1));
        }
        else
        {
            txPacket.append(QString::number(0));
        }

        txPacket.append(" ");
    }

    foreach(QROVServo s, mRov.servos)
    {
        txPacket.append(QString::number(s.value));
        txPacket.append(" ");
    }

    txDatagram = txPacket.toUtf8();

    txSocket->writeDatagram(txDatagram.data(), txDatagram.size(), QHostAddress::Broadcast, TOBIPORT);

    mRov.msSinceEpoch = QDateTime::currentMSecsSinceEpoch(); //timestamp the ROV state
    logRovState();

    emit sentPacket(txPacket);
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

    mRov.piData.tempC = tempC;
    mRov.piData.uptimeS = (int)(uptime);
    mRov.piData.ipAddress = piAddress;
    mRov.piData.usedMemory = usedMemoryPercentage;
    mRov.piData.usedCpu = usedCpuPercentage;

    mutex.unlock();
}

void QROVController::noJoystick()
{
    for(int i=0;i<mRov.motors.count();i++)
    {
        mRov.motors[i].value = 1500; //set everything neutral
    }
}

int QROVController::getPortTOBI() const
{
    return TOBIPORT;
}

int QROVController::getPortTIBO() const
{
    return captureRx->socket()->localPort();
}

int QROVController::getPortRpiTibo() const
{
    return capturePi->socket()->localPort();
}

void QROVController::loadSettings()
{
    QMutex mutex;
    mutex.lock();
    //TODO: Finish adding settings code and remove it from mainwindow.cpp

    //Bilinear
    joy->bilinearEnable(mySettings->value("bilinear/enabled", "1").toBool());
    joy->bilinearConstant(mySettings->value("bilinear/ratio", "1.5").toDouble());

    //Joystick
    joySettings.axisX = mySettings->value("joystick/x", "0").toInt();
    joySettings.axisY = mySettings->value("joystick/y", "0").toInt();
    joySettings.axisZ = mySettings->value("joystick/z", "0").toInt();
    joySettings.axisV = mySettings->value("joystick/v", "0").toInt();
    joySettings.axisL = mySettings->value("joystick/l", "0").toInt();
    joySettings.axisR = mySettings->value("joystick/r", "0").toInt();

    for(int i=0; i<joy->numAxes(); i++)
    {
        Q_ASSERT(joy->deadzone(i, mySettings->value("joystick/dead" +
                                                    QString::number(i),
                                                    "0").toInt()));
    }

    for(int i=0; i<relayMappings.count(); i++)
    {
        relayMappings[i].button = mySettings->value("joystick/but/r" + QString::number(i)).toInt();
        relayMappings[i].hat = mySettings->value("joystick/hat/r" + QString::number(i)).toInt();
        relayMappings[i].dir = mySettings->value("joystick/dir/r" + QString::number(i)).toInt();
    }

    for(int i=0; i<servoMappings.count(); i++)
    {
        servoMappings[i].hatDown = mySettings->value("joystick/hat/s" + QString::number(i) + "/down").toInt();
        servoMappings[i].dirDown = mySettings->value("joystick/dir/s" + QString::number(i) + "/down").toInt();
        servoMappings[i].hatUp = mySettings->value("joystick/hat/s" + QString::number(i) + "/up").toInt();
        servoMappings[i].dirUp = mySettings->value("joystick/dir/s" + QString::number(i) + "/up").toInt();
        servoMappings[i].buttonDown = mySettings->value("joystick/but/s" + QString::number(i) + "/down").toInt();
        servoMappings[i].buttonUp = mySettings->value("joystick/but/s" + QString::number(i) + "/up").toInt();

    }

    myVectorDrive->initVector(MOTORMIN, MOTORMAX);

    //Video
    IpVideoFeed videoFeed = mRov.videoFeed;
    videoFeed.name = mySettings->value("videoFeeds/name", "Main").toString();
    videoFeed.url = mySettings->value("videoFeeds/url", "http://127.0.0.1:8080/javascript_simple.html").toUrl();
    videoFeed.autoGenerate = mySettings->value("videoFeeds/autoGenerate", true).toBool();
    mRov.videoFeed = videoFeed;

    //Logging
    mLoggingEnabled = mySettings->value("logging/history/enabled", true).toBool();

    mutex.unlock();
}

void QROVController::saveSettings()
{
    QMutex mutex;
    mutex.lock();
    //Relay Names
    for(int i=0; i<mRov.relays.count(); i++)
    {
        mySettings->setValue("names/relay" + QString::number(i), mRov.relays[i].name);
    }

    //Bilinear
    mySettings->setValue("bilinear/enabled", joy->bilinearEnabled());
    mySettings->setValue("bilinear/ratio", joy->bilinearConstant());

    //Joystick
    mySettings->setValue("joystick/x", joySettings.axisX);
    mySettings->setValue("joystick/y", joySettings.axisY);
    mySettings->setValue("joystick/z", joySettings.axisZ);
    mySettings->setValue("joystick/v", joySettings.axisV);
    mySettings->setValue("joystick/l", joySettings.axisL);
    mySettings->setValue("joystick/r", joySettings.axisR);

    for(int i=0; i<joy->numAxes(); i++)
    {
        mySettings->setValue("joystick/dead" + QString::number(i),
                             joy->deadzone(i));
    }

    for(int i=0; i<relayMappings.count(); i++)
    {
        mySettings->setValue("joystick/but/r" + QString::number(i), relayMappings[i].button);
        mySettings->setValue("joystick/hat/r" + QString::number(i), relayMappings[i].hat);
        mySettings->setValue("joystick/dir/r" + QString::number(i), relayMappings[i].dir);
    }

    for(int i=0; i<servoMappings.count(); i++)
    {
        mySettings->setValue("joystick/hat/s" + QString::number(i) + "/up", servoMappings[i].hatUp);
        mySettings->setValue("joystick/dir/s" + QString::number(i) + "/up", servoMappings[i].dirUp);
        mySettings->setValue("joystick/hat/s" + QString::number(i) + "/down", servoMappings[i].hatDown);
        mySettings->setValue("joystick/dir/s" + QString::number(i) + "/down", servoMappings[i].dirDown);
        mySettings->setValue("joystick/but/s" + QString::number(i) + "/up", servoMappings[i].buttonUp);
        mySettings->setValue("joystick/but/s" + QString::number(i) + "/down", servoMappings[i].buttonDown);        
    }

    //Video
    mySettings->setValue("videoFeeds/name", mRov.videoFeed.name);
    mySettings->setValue("videoFeeds/url", mRov.videoFeed.url);
    mySettings->setValue("videoFeeds/autoGenerate", mRov.videoFeed.autoGenerate);

    //Logging
    mySettings->setValue("logging/history/enabled", mLoggingEnabled);

    mutex.unlock();
    emit savedSettings("Settings saved", MsgType::Good);
}

void QROVController::motherFunction()
{
    QMutex mutex;
    mutex.lock();

    //If not in a valid state, don't do anything
    if(getValidity())
    {
        if(0 < joy->numJoysticks())
        {
            readMappings();
        }
        else
        {
            noJoystick();
        }
        sendPacket();
        emit onMotherFunctionCompleted();
    }
    mutex.unlock();
}

int QROVController::mapInt(int input, int inMin, int inMax, int outMin, int outMax) const
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
    QList<int> hatStates = joy->hatStates();
    for(int h=0; h < hatStates.count(); h++) //support for multiple hats
    {
        if(hatStates[h] != 0)   //if hat is not neutral
        {
            for(int i=0;i<servoMappings.count();i++)  //for each servo
            {
                if(h == servoMappings[i].hatUp &&
                   hatStates[h] == servoMappings[i].dirUp)  //if increment
                    emit changeServo(i, 1);
                else if(h == servoMappings[i].hatDown &&
                        hatStates[h] == servoMappings[i].dirDown)   //if dec
                    emit changeServo(i, 0);
            }
        }
    }
    for(int s=0; s < servoMappings.count(); s++) //for each servo
    {
        for(int i=0; i < joy->buttonStates().count();i++) //for each button
        {
            if(joy->buttonStates()[i] && servoMappings[s].buttonUp == i) //if the up button is pressed
            {
                emit changeServo(s, 1);   //increment
            }
            else if(joy->buttonStates()[i] && servoMappings[s].buttonDown == i) //if the down button is pressed
            {
                emit changeServo(s, 0); //decrement
            }
        }
    }
    mutex.unlock();
}

void QROVController::diveTimeReset()
{
    diveTimer->reset();
    emit appendToActivityMonitor("Dive timer reset", MsgType::Warn);
}

QString QROVController::diveTimeString() const
{
    return diveTimer->diveTimeString();
}

void QROVController::setValidity(bool state)
{
   mValidity = state;
}

bool QROVController::getValidity() const
{
   return mValidity;
}

void QROVController::logRovState()
{
    if(isLoggingEnabled())
    {
        rovHistory.push_back(mRov);
    }
}

int QROVController::getJoyId() const
{
    return joy->currentJoystick();
}

int QROVController::getNumberJoysticks() const
{
    return joy->numJoysticks();
}

const QList<int>& QROVController::getJoystickAxesValues() const
{
    return joy->axisStates();
}

const QList<bool>& QROVController::getJoystickButtonsPressed() const
{
    return joy->buttonStates();
}

QList<int> QROVController::getJoystickHatsPressed() const
{
    return joy->hatStates();
}

void QROVController::setBilinearRatio(double r)
{
    joy->bilinearConstant(r);
}

double QROVController::getBilinearRatio() const
{
    return joy->bilinearConstant();
}

void QROVController::setBilinearEnabled(bool b)
{
    joy->bilinearEnable(b);
}

bool QROVController::getBilinearEnabled() const
{
    return joy->bilinearEnabled();
}

void QROVController::setXDeadzone(int x)
{
    Q_ASSERT(0 <= joySettings.axisX &&
            joySettings.axisX < joy->numAxes());
    joy->deadzone(joySettings.axisX, x);
}

int QROVController::getXDeadzone() const
{
    Q_ASSERT(0 <= joySettings.axisX &&
            joySettings.axisX < joy->numAxes());
    return joy->deadzone(joySettings.axisX);
}

void QROVController::setYDeadzone(int y)
{
    Q_ASSERT(0 <= joySettings.axisY &&
            joySettings.axisY < joy->numAxes());
    joy->deadzone(joySettings.axisY, y);
}

int QROVController::getYDeadzone() const
{
    Q_ASSERT(0 <= joySettings.axisY &&
            joySettings.axisY < joy->numAxes());
    return joy->deadzone(joySettings.axisY);
}

void QROVController::setZDeadzone(int z)
{
    Q_ASSERT(0 <= joySettings.axisZ &&
            joySettings.axisZ  < joy->numAxes());
    joy->deadzone(joySettings.axisZ, z);
}

int QROVController::getZDeadzone() const
{
    Q_ASSERT(0 <= joySettings.axisZ &&
            joySettings.axisZ < joy->numAxes());
    return joy->deadzone(joySettings.axisZ);
}

bool QROVController::isJoyAttached() const
{
    return joy->joystickAttached();
}
