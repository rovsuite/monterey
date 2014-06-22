#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include "qactivitymonitor.h"
#include "qboolmonitor.h"
#include "rovabout.h"
#include "rovdebug.h"
#include "rovmappings.h"
#include "rovsettings.h"
#include "qrovcontroller.h"
#include <QDebug>
#include <QSettings>
#include <QPointF>
#include <QThread>
#include <QBrush>
#include <QColor>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QGroupBox>
#include <QLCDNumber>
#include <QTextEdit>
#include <QWebView>
#include <QClipboard>

#include "extraclasses/DepthTape/depthtape.h"
#include "extraclasses/Compass/compass.h"
#include "extraclasses/LedIndicator/ledindicator.h"

namespace Ui {
    class MainWindow;
}

typedef QROVController::MsgType MsgType;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QROVController *controller;

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);

public slots:
    void loadSettings();    //!< load the application's settings

    void setupCustomWidgets();  //!< Setup the custom widgets

    void onCalledClickRelayButton(QPushButton * button);

    void onCalledServoChange(int id, int direction);

    void appendToActivityMonitor(QString message, MsgType type);

    void saveRovLogFile();
    void clearLog();

private slots:
    //Buttons
    void on_pbRelay_clicked();
    void on_buttonCopyLogToClipboard_clicked();
    void on_buttonClearLog_clicked();

    //Sliders
    void on_vsServo_valueChanged(int value);

    //Show dialogs
    void showAbout();   //!< show the about dialog
    void showDebug();   //!< show the debug dialog
    void showMappings();    //!< show the joystick mapping dialog
    void showSettings();    //!< show the settings dialog

    //Show misc. values
    void refreshGUI();  //!< Refresh the GUI on a timer
    void showDiveTimer();   //!< Display the dive timer
    void onComTiboChanged(bool status);    //!< Add TIBO lost/gained to the activity monitor
    void onComPiChange(bool status); //!< Add RPi COM lost/gained to the activity monitor
    void displayTime(); //!< Display the current time
    void lostJoystick();    //!< Add joystick lost/gained to the activity monitor

    //Grab new data from the ROV controller
    void loadData();    //!< Load the data from the ROV controller and display it

    //Setup QML widgets
    void setupDepthTape();  //!< Configure the depth tape
    void setupCompass();    //!< Configure the compass
    void loadUiGeometry();

    //Misc functions
    void checkForUpdates(); //!< Check online for updates to Monterey
    void showFullscreen(bool fullscreen);
    void zoomTheCameraFeed(int zoomAmount);

private:
    QString *version;

    Ui::MainWindow *ui;
    QActivityMonitor *activityMonitor;
    ROVAbout *dialogAbout;
    ROVDebug *dialogDebug;
    ROVMappings *dialogMappings;
    ROVSettings *dialogSettings;
    QSettings *mySettings;

    QList<QPushButton*> relayButtons;
    QList<QSlider*> servoSliders;
    QList<QLCDNumber*> sensorDisplays;

    QTime *graphTime;
    DepthTape *depthTape;
    Compass *compass;
    QWebView* webCamViewer;

    QTimer *guiTimer;

    QThread *engineThread;

    struct StatusLights {
    LedIndicator *com;
    LedIndicator *joystick;
    LedIndicator *rPi;
    }statusLights;
};

#endif // MAINWINDOW_H
