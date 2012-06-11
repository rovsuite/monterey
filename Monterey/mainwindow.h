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

#ifdef Q_OS_MACX
#include "qwt/qwt_dial.h"
#include "qwt/qwt_dial_needle.h"
#include "qwt/qwt_compass.h"
#include "qwt/qwt_compass_rose.h"
#include "qwt/qwt_plot_marker.h"
#include "qwt/qwt_abstract_scale_draw.h"
#include "qwt/qwt_scale_draw.h"
#include "qwt/qwt.h"
#include "qwt/qwt_plot_curve.h"
#include "qwt/qwt_plot.h"
#include "qwt/qwt_series_data.h"
#include "qwt/qwt_plot_curve.h"
#endif

#ifdef Q_OS_WIN32
#include "qwt_dial.h"
#include "qwt_dial_needle.h"
#include "qwt_compass.h"
#include "qwt_compass_rose.h"
#include "qwt_plot_marker.h"
#include "qwt_abstract_scale_draw.h"
#include "qwt_scale_draw.h"
#include "qwt.h"
#include "qwt_plot_curve.h"
#include "qwt_plot.h"
#include "qwt_series_data.h"
#include "qwt_plot_curve.h"
#endif

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

public slots:
    void loadSettings();    //!< load the application's settings

    void setupCustomWidgets();  //!< Setup the custom widgets

private slots:
    void on_pbRelay0_clicked();

    void on_pbRelay1_clicked();

    void on_pbRelay2_clicked();

    void on_vsServo0_valueChanged(int value);

    void on_vsServo1_valueChanged(int value);

    void showAbout();   //!< show the about dialog
    void showDebug();   //!< show the debug dialog
    void showMappings();    //!< show the joystick mapping dialog
    void showSettings();    //!< show the settings dialog
    void showDiveTimer();   //!< Display the dive timer
    void ledDisplay();  //!< Light up the LEDs based on each important status
    void noErrorTOBI(); //!< Light up the green TOBI LED
    void noErrorTIBO(); //!< Light up the green TIBO LED
    void errorTOBI();    //!< Light up the red TOBI LED
    void errorTIBO();   //!< Light up the red TIBO LED
    void lostTOBI();    //!< Add TOBI lost/gained to the activity monitor
    void lostTIBO();    //!< Add TIBO lost/gained to the activity monitor
    void thresholdCheck();  //!< Check the input values to see if they reach thresholds and alert the user if they do
    void loadData();    //!< Load the data from the ROV controller and display it
    void displayTime(); //!< Display the current time
    void refreshGUI();  //!< Refresh the GUI on a timer
    void lostJoystick();    //!< Add joystick lost/gained to the activity monitor

private:
    QString *version;

    Ui::MainWindow *ui;
    QActivityMonitor *activityMonitor;
    ROVAbout *dialogAbout;
    ROVDebug *dialogDebug;
    ROVMappings *dialogMappings;
    ROVSettings *dialogSettings;
    QSettings *mySettings;

    QwtPlotCurve *depthCurve;
    QVector<QPointF> depthPoints;
    QTime *graphTime;
    int graphDepthIndex;

    QTimer *guiTimer;
};

#endif // MAINWINDOW_H
