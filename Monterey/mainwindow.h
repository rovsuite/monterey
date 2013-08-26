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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QROVController *controller;

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void loadSettings();    //!< load the application's settings

    void setupCustomWidgets();  //!< Setup the custom widgets

    void onCalledClickRelayButton(QPushButton * button);

    void onCalledServoChange(int id, int direction);

    void appendToActivityMonitor(QString message);

private slots:
    //Buttons
    void on_pbRelay0_clicked();
    void on_pbRelay1_clicked();
    void on_pbRelay2_clicked();
    void on_buttonCopyLogToClipboard_clicked();
    void on_buttonClearLog_clicked();

    //Sliders
    void on_vsServo0_valueChanged(int value);
    void on_vsServo1_valueChanged(int value);

    //Show dialogs
    void showAbout();   //!< show the about dialog
    void showDebug();   //!< show the debug dialog
    void showMappings();    //!< show the joystick mapping dialog
    void showSettings();    //!< show the settings dialog

    //Show misc. values
    void refreshGUI();  //!< Refresh the GUI on a timer
    void displayTahoe();    //!< Display the values sent over by Tahoe so that the use knows what's going on
    void showDiveTimer();   //!< Display the dive timer
    void onComTiboChanged(bool status);    //!< Add TIBO lost/gained to the activity monitor
    void onComTahoeChanged(bool status);   //!< Add Tahoe COM lost/gained to the activity monitor
    void onComPiChange(bool status); //!< Add RPi COM lost/gained to the activity monitor
    void displayTime(); //!< Display the current time
    void lostJoystick();    //!< Add joystick lost/gained to the activity monitor

    //Grab new data from the ROV controller
    void loadData();    //!< Load the data from the ROV controller and display it

    //Check sensor thresholds (MIGHT REMOVE)
    void thresholdCheck();  //!< Check the input values to see if they reach thresholds and alert the user if they do

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

    QVector<double> depthPoints;
    QVector<double> voltagePoints;
    QVector<double> rPiCpuTempCPoints;
    QVector<double> sensor0Points;
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
    LedIndicator *tahoe;
    }statusLights;
};

#endif // MAINWINDOW_H
