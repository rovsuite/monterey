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
#include "qrov.h"
#include "qjoystick.h"
#include "extraclasses/QVectorDrive2/qvectordrive2.h"
#include "extraclasses/QBoolMonitor/qboolmonitor.h"

class QROVController : public QObject
{
    Q_OBJECT
public:
    explicit QROVController(QObject *parent = 0);
    QROV *rov;
    QList<int> myVector;    //hold the motor values

    QBoolMonitor *monitorTOBI;
    QBoolMonitor *monitorTIBO;
    QBoolMonitor *monitorJoystick;
    enum MotorLayout{vectorDrive, tankDrive};

signals:
    void sentPacket(QString packet);
    void receivedPacket(QString packet);

    void savedSettings(QString message);

    void errorTOBI();
    void errorTIBO();
    void noErrorTOBI();
    void noErrorTIBO();

public slots:
    void motherFunction();  //!< Used to loop the application

    //Joystick
    void initJoysticks();   //!< Initialize joystick systems
    void updateJoystickData();  //!< Read joystick data
    void readMappings();    //!< Read adjustable mappings
    void rescanJoysticks(); //!< Reenumerate joysticks
    QStringList getJoystickNames();  //!< Get the names of the joysticks

    //Networking
    void processPacket();   //!< Process the packet received from the ROV
    void sendPacket();  //!< Send a packet to the ROV
    void sendDebug();   //!< Send a debug packet out to any listeners

    //ROVContoller interactive functions
    void setBilinearThreshold(double t) { bilinearThreshold = t; }
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
    double getMaxDepth();
    int getJoysAvail() { joysAvail = joy->availableJoysticks(); return joysAvail; }    //!< Return the number of attached joysticks
    bool isJoyAttached() {return joyAttached; } //!< Return the joystick attached status
    bool getStatusTOBI() { return comTOBI; }    //!< Return the status of TOBI
    bool getStatusTIBO() { return comTIBO; }    //!< Return the status of TIBO
    MotorLayout getMotorLayout() { return motorLayout; }  //!< Return the currently set motor layout
    void setMotorLayout(MotorLayout mL) { motorLayout = mL; }   //!< Set the motor layout
    void setMotorLayout(int mL) { motorLayout = (MotorLayout)mL; }   //!< Set the motor layout
    void loadSettings();    //!< Force a loading of the settings
    void saveSettings();    //!< Force a saving of the settings

    //Error functions
    void setErrorTOBI();    //!< use to say that there is a TOBI error
    void setErrorTIBO();   //!< use to say that there is a TIBO error

private slots:
    //Misc
    int mapInt(int input, int inMin, int inMax, int outMin, int outMax);    //!< map a value from one range to another range, stolen from http://arduino.cc

private:
    QJoystick *joy; //joystick object
    QVectorDrive2 *myVectorDrive;   //vector drive object
    QSettings *mySettings;
    QUdpSocket *rxSocket;
    QUdpSocket *txSocket;
    unsigned int tiboPort;
    unsigned int tobiPort;

    bool comTOBI;
    bool comTIBO;
    QTimer *timerTOBI;
    QTimer *timerTIBO;
    int joysAvail;
    bool joyAttached;

    bool bilinearEnabled;
    bool vectorEnabled;
    double bilinearThreshold;
    double bilinearRatio;
    int xDead;
    int yDead;
    int zDead;
    MotorLayout motorLayout;

};

#endif // QROVCONTROLLER_H
