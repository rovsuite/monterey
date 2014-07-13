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
#include <QQueue>
#include "qrov.h"
#include "../../extraclasses/QVectorDrive2/qvectordrive2.h"
#include "../../extraclasses/QBoolMonitor/qboolmonitor.h"
#include "../../extraclasses/DiveTimer/divetimer.h"
#include "../../extraclasses/UdpCapture/udpcapture.h"

/* Settings area where the user can tweak monterey for their own personal
 * use.  Please change these numbers as you see fit.  The timeout values
 * are measured in milliseconds.  Please note that any change in motors,
 * relays or servos will need to be manually propagated to the UI.
 */

#define MOTORMIN 1000
#define MOTORMAX 2000

#define SERVOMIN 0
#define SERVOMAX 179

#define ERRORTIMEOUT 500
#define PITIMEOUT 5000
#define TOBIPORT 51000
#define TIBOPORT 50000
#define PIRXPORT 5060

/*
 *  END OF SETTINGS AREA
 */

class QJoystickInterface;
class QROVController : public QObject
{
    Q_OBJECT
public:
    enum MsgType { Info, Good, Warn, Bad };

    explicit QROVController(MsgType& status, QString& statusMessage, QObject *parent = 0);
    ~QROVController();

    QBoolMonitor *monitorJoystick;

    //Struct to map user input (UI and JS) to relays
    struct RelayMapping
    {
        int button;
        int hat;
        int dir;
        QPushButton *pushButton;

        RelayMapping(int b, int h, int d, QPushButton *pb)
        {
            button = b;
            hat = h;
            dir = d;
            pushButton = pb;
        }

        RelayMapping()
        {
            button = -1;
            hat = -1;
            dir = -1;
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
        int dirUp;
        int hatDown;
        int dirDown;
        QSlider *slider;

        ServoMapping(int bUp, int bDown,
                     int hUp, int hDown,
                     int dUp, int dDown, QSlider *s)
        {
            buttonUp = bUp;
            buttonDown = bDown;
            hatUp = hUp;
            dirUp = dUp;
            hatDown = hDown;
            dirDown = dDown;
            slider = s;
        }

        ServoMapping()
        {
            buttonUp = -1;
            buttonDown = -1;
            hatUp = -1;
            dirUp = -1;
            hatDown = -1;
            dirDown = -1;
            slider = nullptr;
        }
    };
    QList<ServoMapping> servoMappings;

signals:
    void onMotherFunctionCompleted();

    //Networking
    void sentPacket     (QString packet);
    void receivedPacket (QString packet);
    void comTiboChanged (bool status);
    void comPiChanged   (bool status);

    //Misc UI Interactions
    void clickRelayButton        (QPushButton * pb);
    void changeServo             (int id, int direction);
    void appendToActivityMonitor (QString message, MsgType type);

    //Misc
    void savedSettings(QString message, MsgType type);

public slots:

    const QROV& rov() const { return mRov; }
    QROV& editRov()         { return mRov; }

    //ROV Log functions
    bool saveRovLog(const QString& filename);
    bool isLoggingEnabled() const { return mLoggingEnabled; }
    void enableLogging(bool enable);
    void clearLog();
    bool logHasItems() const { return !rovHistory.empty(); }

    //Joystick
    void rescanJoysticks(); //!< Reenumerate joysticks
    QStringList getJoystickNames();  //!< Get the names of the joysticks
    const QList<int>&  getJoystickAxesValues() const;
    QList<int>  getJoystickHatsPressed() const;
    const QList<bool>& getJoystickButtonsPressed() const;
    int getJoyId() const;
    int getNumberJoysticks() const;
    bool isJoyAttached() const; //!< Return the joystick attached status
    int getAxisX() const { return joySettings.axisX; }
    int getAxisY() const { return joySettings.axisY; }
    int getAxisZ() const { return joySettings.axisZ; }
    int getAxisV() const { return joySettings.axisV; }
    int getAxisL() const { return joySettings.axisL; }
    int getAxisR() const { return joySettings.axisR; }
    void setAxisX(int x) { joySettings.axisX = x; }
    void setAxisY(int y) { joySettings.axisY = y; }
    void setAxisZ(int z) { joySettings.axisZ = z; }
    void setAxisV(int v) { joySettings.axisV = v; }
    void setAxisL(int l) { joySettings.axisL = l; }
    void setAxisR(int r) { joySettings.axisR = r; }

    //Catch Joystick signals
    void onButtonPressed  (int button);
    void onButtonReleased (int button);
    void onButtonToggled  (int button, bool state);
    void onHatPressed     (int hat, int dir);
    void onHatReleased    (int hat, int dir);
    void onHatToggled     (int hat, int dir, bool state);
    void onAxesUpdated    (const QList<int>& values);

    //Motor math
    void setBilinearRatio  (double r);
    void setBilinearEnabled(bool b);
    double getBilinearRatio() const;
    bool getBilinearEnabled() const;
    int getXDeadzone() const;
    int getYDeadzone() const;
    int getZDeadzone() const;
    void setXDeadzone(int x);
    void setYDeadzone(int y);
    void setZDeadzone(int z);

    //Networking
    int getPortTOBI() const;  //!< Get the TOBI port
    int getPortTIBO() const;  //!< Get the TIBO port
    int getPortRpiTibo() const; //!< Get the port that the Raspberry Pi sends to
    bool getStatusTIBO() const { return captureRx->comStatus(); }    //!< Return the status of TIBO
    bool getStatusPi() const { return capturePi->comStatus(); }

    //Settings
    void loadSettings();    //!< Force a loading of the settings
    void saveSettings();    //!< Force a saving of the settings

    //Dive timer
    void diveTimeReset();   //!< Reset the dive timer
    QString diveTimeString() const; //!< Convert the milliseconds to minutes and hours and display in the gui

private slots:
    void motherFunction();  //!< Used to loop the application

    void setValidity(bool state);
    bool getValidity() const;

    void logRovState(); //Save ROV state for writting to a log later

    //Joystick
    void initJoysticks();   //!< Initialize joystick systems
    void readMappings();    //!< Read adjustable mappings
    void noJoystick();   //!< Put the ROV motors into a neutral state if the joystick is not attached

    //Networking
    void processPacket(QString packet);   //!< Process the packet received from the ROV
    void sendPacket();  //!< Send a packet to the ROV
    void processPi(QString packet);

    //Misc
    int mapInt(int input, int inMin, int inMax, int outMin, int outMax) const;    //!< map a value from one range to another range, stolen from http://arduino.cc

private:
    QSettings *mySettings;

    //ROV State and logging
    QROV mRov;
    QQueue<QROV> rovHistory;
    bool mLoggingEnabled;

    bool mValidity; //is the controller in a valid state

    //Networking
    UdpCapture *captureRx;
    UdpCapture *capturePi;
    QUdpSocket *txSocket;

    //Joystick
    QJoystickInterface *joy; //joystick object

    //Timers
    DiveTimer *diveTimer;
    QTimer *packetTimer;

    //Motor math
    QVectorDrive2 *myVectorDrive;   //vector drive object

    struct JoystickSettings
    {
        //Axes
        int axisX;
        int axisY;
        int axisZ;
        int axisV;
        int axisL;
        int axisR;

        //Motor value math settings
        bool vectorEnabled;
    }joySettings;
};

#endif // QROVCONTROLLER_H
