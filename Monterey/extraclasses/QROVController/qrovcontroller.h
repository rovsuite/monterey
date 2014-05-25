#ifndef QROVCONTROLLER_H
#define QROVCONTROLLER_H

//Written by Chris Konstad
//March 2012

#include <QObject>
#include <QSettings>
#include <QByteArray>
#include <QString>
#include <QUdpSocket>
#include <QtNetwork>
#include <QTimer>
#include <QThread>
#include <QTime>
#include <QMutex>
#include <QPushButton>
#include <QSlider>
#include "qrov.h"
#include "qjoystick.h"
#include "../../extraclasses/QVectorDrive2/qvectordrive2.h"
#include "../../extraclasses/QBoolMonitor/qboolmonitor.h"
#include "../../extraclasses/DiveTimer/divetimer.h"
#include "../../extraclasses/PiData/pidata.h"
#include "../../extraclasses/UdpCapture/udpcapture.h"

/* Settings area where the user can tweak monterey for their own personal
 * use.  Please change these numbers as you see fit.  The timeout values
 * are measured in milliseconds.  Please note that any change in motors,
 * relays or servos will need to be manually propagated to the UI.
 */

#define numberOfMotors 6
#define numberOfRelays 3
#define numberOfServos 3

#define MOTORMIN 1000
#define MOTORMAX 2000

#define SERVOMIN 0
#define SERVOMAX 179

#define ERRORTIMEOUT 500
#define PITIMEOUT 5000
#define TOBIPORT 51000
#define TIBOPORT 50000
#define TAHOERXPORT 52000
#define TAHOETXPORT 53000
#define PIRXPORT 5060

/*
 *  END OF SETTINGS AREA
 */

class QROVController : public QObject
{
    Q_OBJECT
public:
    explicit QROVController(QObject *parent = 0);

    QROV *rov;
    QList<int> joystickAxesValues;
    QBoolMonitor *monitorJoystick;

    //Struct to map user input (UI and JS) to relays
    struct RelayMapping
    {
        int button;
        int hat;
        QPushButton *pushButton;

        RelayMapping(int b, int h, QPushButton *pb)
        {
            button = b;
            hat = h;
            pushButton = pb;
        }

        RelayMapping()
        {
            button = -1;
            hat = -1;
            pushButton = nullptr;
        }
    };
    QList<RelayMapping> relayMappings;

    //Struct to map user input (UI and JS) to relays
    struct ServoMapping
    {
        int buttonUp;
        int buttonDown;
        int hatUp;
        int hatDown;
        QSlider *slider;

        ServoMapping(int bUp, int bDown,
                     int hUp, int hDown, QSlider *s)
        {
            buttonUp = bUp;
            buttonDown = bDown;
            hatUp = hUp;
            hatDown = hDown;
            slider = s;
        }

        ServoMapping()
        {
            buttonUp = -1;
            buttonDown = -1;
            hatUp = -1;
            hatDown = -1;
            slider = nullptr;
        }
    };
    QList<ServoMapping> servoMappings;

signals:
    void onMotherFunctionCompleted();

    //Networking
    void sentPacket(QString packet);
    void receivedPacket(QString packet);
    void onTahoeProcessed();
    void comTiboChanged(bool status);
    void comTahoeChanged(bool status);
    void comPiChanged(bool status);

    //Misc UI Interactions
    void clickRelayButton(QPushButton * pb);
    void changeServo(int id, int direction);
    void appendToActivityMonitor(QString message);

    //Misc
    void savedSettings(QString message);

public slots:
    //Joystick
    void rescanJoysticks(); //!< Reenumerate joysticks
    QStringList getJoystickNames();  //!< Get the names of the joysticks
    int getJoystickNumberAxes() { return numberOfAxes; }   //!< Get the number of axes
    int getJoystickNumberButtons() { return joy->joystickNumButtons(0); } //!< Get the number of buttons
    int getJoystickCurrentHatValue();    //!< Get the current hat value
    QList<int> getJoystickCurrentButtonValue();    //!< Get the (first in list) currently pressed button if one is pressed
    int getJoyID() { return joyID; }    //!< get the ID of the currently selected joystick
    void setJoyID(int j) { joyID = j; } //!< set the joystick to be read
    int getJoystickAxesValues(int index) { return joystickAxesValues[index]; }
    int getJoysAvail() { joysAvail = joy->availableJoysticks(); return joysAvail; }    //!< Return the number of attached joysticks
    bool isJoyAttached() { return joyAttached; } //!< Return the joystick attached status
    int getAxisX() { return joySettings.axisX; }    //!< Get the axis x ID
    void setAxisX(int x) { joySettings.axisX = x; }
    int getAxisY() { return joySettings.axisY; }
    void setAxisY(int y) { joySettings.axisY = y; }
    int getAxisZ() { return joySettings.axisZ; }
    void setAxisZ(int z) { joySettings.axisZ = z; }
    int getAxisV() { return joySettings.axisV; }
    void setAxisV(int v) { joySettings.axisV = v; }
    int getAxisL() { return joySettings.axisL; }
    void setAxisL(int l) { joySettings.axisL = l; }
    int getAxisR() { return joySettings.axisR; }
    void setAxisR(int r) { joySettings.axisR = r; }

    //Motor math
    void setBilinearRatio(double r) { joySettings.bilinearRatio = r; }
    double getBilinearRatio() { return joySettings.bilinearRatio; }
    void setBilinearEnabled(bool b) { joySettings.bilinearEnabled = b; }
    bool getBilinearEnabled() { return joySettings.bilinearEnabled; }
    void setXDeadzone(int x) { joySettings.deadX = x; }
    int getXDeadzone() { return joySettings.deadX; }
    void setYDeadzone(int y) { joySettings.deadY = y; }
    int getYDeadzone() { return joySettings.deadY; }
    void setZDeadzone(int z) { joySettings.deadZ = z; }
    int getZDeadzone() { return joySettings.deadZ; }

    //Networking
    int getPortTOBI();  //!< Get the TOBI port
    int getPortTIBO();  //!< Get the TIBO port
    int getPortRpiTibo(); //!< Get the port that the Raspberry Pi sends to
    bool getStatusTIBO() { return captureRx->comStatus(); }    //!< Return the status of TIBO
    bool getStatusTahoe() { return captureTahoe->comStatus(); }  //!< Return the status of Tahoe's COM
    bool getStatusPi() { return capturePi->comStatus(); }

    //Settings
    void loadSettings();    //!< Force a loading of the settings
    void saveSettings();    //!< Force a saving of the settings


    //Dive timer
    void diveTimeReset();   //!< Reset the dive timer
    QString diveTimeString(); //!< Convert the milliseconds to minutes and hours and display in the gui

private slots:
    void motherFunction();  //!< Used to loop the application

    //Joystick
    void initJoysticks();   //!< Initialize joystick systems
    void updateJoystickData();  //!< Read joystick data
    void readMappings();    //!< Read adjustable mappings
    void noJoystick();   //!< Put the ROV motors into a neutral state if the joystick is not attached
    void joystickButtonClicked(int buttonID);
    void joystickHatClicked(int hatID);

    //Networking
    void processPacket(QString packet);   //!< Process the packet received from the ROV
    void sendPacket();  //!< Send a packet to the ROV
    void sendDebug();   //!< Send a debug packet out to any listeners
    void processTahoe(QString packet);    //!< Process the packet from Tahoe
    void sendTahoe();   //!< Send the packet to Tahoe
    void processPi(QString packet);

    //Misc
    int mapInt(int input, int inMin, int inMax, int outMin, int outMax);    //!< map a value from one range to another range, stolen from http://arduino.cc

private:
    QSettings *mySettings;

    //Networking
    UdpCapture *captureRx;
    UdpCapture *captureTahoe;
    UdpCapture *capturePi;
    QUdpSocket *txSocket;

    //Joystick
    QJoystick *joy; //joystick object
    int joysAvail;
    int joyID;
    bool joyAttached;
    int numberOfAxes;

    //Timers
    DiveTimer *diveTimer;
    QTimer *packetTimer;

    //Motor math
    QVectorDrive2 *myVectorDrive;   //vector drive object

    struct JoystickSettings
    {
        //Deadzones
        int deadX;
        int deadY;
        int deadZ;

        //Axes
        int axisX;
        int axisY;
        int axisZ;
        int axisV;
        int axisL;
        int axisR;

        //Motor value math settings
        bool vectorEnabled;
        bool bilinearEnabled;
        double bilinearRatio;
    }joySettings;
};

#endif // QROVCONTROLLER_H
