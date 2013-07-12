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
#include "qrov.h"
#include "qjoystick.h"
#include "extraclasses/QVectorDrive2/qvectordrive2.h"
#include "extraclasses/QBoolMonitor/qboolmonitor.h"
#include "extraclasses/DiveTimer/divetimer.h"

class QROVController : public QObject
{
    Q_OBJECT
public:
    explicit QROVController(QObject *parent = 0);
    QROV *rov;
    QList<int> myVector;    //hold the motor values
    QList<int> joystickAxesValues;

    QBoolMonitor *monitorTOBI;
    QBoolMonitor *monitorTIBO;
    QBoolMonitor *monitorJoystick;
    enum MotorLayout{vectorDrive, tankDrive};

signals:
    void sentPacket(QString packet);
    void receivedPacket(QString packet);

    void savedSettings(QString message);

    void onMotherFunctionCompleted();
    void onTahoeProcessed();

    void errorTOBI();
    void errorTIBO();
    void noErrorTOBI();
    void noErrorTIBO();

    void clickRelayButton(QPushButton * pb);
    void changeServo(int id, int direction);

public slots:
    void motherFunction();  //!< Used to loop the application

    //Joystick
    void initJoysticks();   //!< Initialize joystick systems
    void updateJoystickData();  //!< Read joystick data
    void readMappings();    //!< Read adjustable mappings
    void rescanJoysticks(); //!< Reenumerate joysticks
    QStringList getJoystickNames();  //!< Get the names of the joysticks
    int getJoystickNumberAxes() { return numberOfAxes; }   //!< Get the number of axes
    int getJoystickNumberButtons() { return joy->joystickNumButtons(0); } //!< Get the number of buttons
    int getJoystickCurrentHatValue();    //!< Get the current hat value
    QList<int> getJoystickCurrentButtonValue();    //!< Get the (first in list) currently pressed button if one is pressed
    int getJoyID() { return joyID; }    //!< get the ID of the currently selected joystick
    void setJoyID(int j) { joyID = j; } //!< set the joystick to be read
    int getJoystickAxesValues(int index) { return joystickAxesValues[index]; }

    //Networking
    void processPacket();   //!< Process the packet received from the ROV
    void sendPacket();  //!< Send a packet to the ROV
    void sendDebug();   //!< Send a debug packet out to any listeners
    void processTahoe();    //!< Process the packet from Tahoe
    void sendTahoe();   //!< Send the packet to Tahoe

    //ROVContoller interactive functions
    double getBilinearThreshold() { return bilinearThreshold; }
    void setBilinearRatio(double r) { bilinearRatio = r; }
    double getBilinearRatio() { return bilinearRatio; }
    void setBilinearEnabled(bool b) { bilinearEnabled = b; }
    bool getBilinearEnabled() { return bilinearEnabled; }
    void setXDeadzone(int x) { xDead = x; }
    int getXDeadzone() { return xDead; }
    void setYDeadzone(int y) { yDead = y; }
    int getYDeadzone() { return yDead; }
    void setZDeadzone(int z) { zDead = z; }
    int getZDeadzone() { return zDead; }
    void noJoystick();   //!< Put the ROV motors into a neutral state if the joystick is not attached
    int getPortTOBI();  //!< Get the TOBI port
    int getPortTIBO();  //!< Get the TIBO port
    int getJoysAvail() { joysAvail = joy->availableJoysticks(); return joysAvail; }    //!< Return the number of attached joysticks
    bool isJoyAttached() {return joyAttached; } //!< Return the joystick attached status
    bool getStatusTOBI() { return comTOBI; }    //!< Return the status of TOBI
    bool getStatusTIBO() { return comTIBO; }    //!< Return the status of TIBO
    bool getStatusTahoe() { return comTahoe; }  //!< Return the status of Tahoe's COM
    MotorLayout getMotorLayout() { return motorLayout; }  //!< Return the currently set motor layout
    void setMotorLayout(MotorLayout mL) { motorLayout = mL; }   //!< Set the motor layout
    void setMotorLayout(int mL) { motorLayout = (MotorLayout)mL; }   //!< Set the motor layout
    void loadSettings();    //!< Force a loading of the settings
    void saveSettings();    //!< Force a saving of the settings
    int getAxisX() { return axisX; }    //!< Get the axis x ID
    void setAxisX(int x) { axisX = x; }
    int getAxisY() { return axisY; }
    void setAxisY(int y) { axisY = y; }
    int getAxisZ() { return axisZ; }
    void setAxisZ(int z) { axisZ = z; }
    int getAxisV() { return axisV; }
    void setAxisV(int v) { axisV = v; }
    int getAxisL() { return axisL; }
    void setAxisL(int l) { axisL = l; }
    int getAxisR() { return axisR; }
    void setAxisR(int r) { axisR = r; }
    void joystickButtonClicked(int buttonID);
    void joystickHatClicked(int hatID);


    //Error functions
    void setErrorTOBI();    //!< use to say that there is a TOBI error
    void setErrorTIBO();   //!< use to say that there is a TIBO error
    void setErrorTahoe();   //!< use to say that Monterey is getting packets from Monterey

    //Dive timer
    void diveTimeReset();   //!< Reset the dive timer
    QString diveTimeString(); //!< Convert the milliseconds to minutes and hours and display in the gui

private slots:
    //Misc
    int mapInt(int input, int inMin, int inMax, int outMin, int outMax);    //!< map a value from one range to another range, stolen from http://arduino.cc

private:
    QJoystick *joy; //joystick object
    QVectorDrive2 *myVectorDrive;   //vector drive object
    QSettings *mySettings;
    QUdpSocket *rxSocket;
    QUdpSocket *txSocket;
    QUdpSocket *tahoeSocket;
    unsigned int tiboPort;
    unsigned int tobiPort;

    bool comTOBI;
    bool comTIBO;
    bool comTahoe;
    QTimer *timerTOBI;
    QTimer *timerTIBO;
    QTimer *timerTahoe;
    int joysAvail;
    int joyID;
    bool joyAttached;
    int numberOfAxes;

    DiveTimer *diveTimer;
    QTimer *packetTimer;

    bool bilinearEnabled;
    bool vectorEnabled;
    double bilinearThreshold;
    double bilinearRatio;
    int xDead;
    int yDead;
    int zDead;
    int axisX;
    int axisY;
    int axisZ;
    int axisV;
    int axisL;
    int axisR;
    MotorLayout motorLayout;

};

#endif // QROVCONTROLLER_H
