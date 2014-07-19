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
    //!< load the application's settings
    void loadSettings();

    //!< Setup the custom widgets
    void setupCustomWidgets();

    void onCalledClickRelayButton(QPushButton * button);

    void onCalledServoChange(int id, int direction);

    void appendToActivityMonitor(QString message, MsgType type);

    void saveRovLogFile();
    void clearLog();

    void onGearChanged(int gear);

private slots:
    // Buttons
    void on_pbRelay_clicked();
    void on_buttonCopyLogToClipboard_clicked();
    void on_buttonClearLog_clicked();

    // Sliders
    void on_vsServo_valueChanged(int value);

    // Show dialogs
    void showAbout();
    void showDebug();
    void showMappings();
    void showSettings();

    // Show misc. values
    //!< Refresh the GUI on a timer
    void refreshGUI();

    //!< Display the dive timer
    void showDiveTimer();

    //!< Add TIBO lost/gained to the activity monitor
    void onComTiboChanged(bool status);

    //!< Add RPi COM lost/gained to the activity monitor
    void onComPiChange(bool status);

    //!< Display the current time
    void displayTime();

    //!< Add joystick lost/gained to the activity monitor
    void lostJoystick();

    //Grab new data from the ROV controller
    void loadData();

    //Setup QML widgets
    void setupDepthTape();
    void setupCompass();
    void loadUiGeometry();

    //Misc functions
    void checkForUpdates();
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
    LedIndicator *gear;
    }statusLights;
};

#endif // MAINWINDOW_H
