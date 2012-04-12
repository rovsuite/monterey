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
    void showAbout();   //!< show the about dialog
    void showDebug();   //!< show the debug dialog
    void showMappings();    //!< show the joystick mapping dialog
    void showSettings();    //!< show the settings dialog

    void loadSettings();    //!< load the application's settings

    void setupCustomWidgets();  //!< Setup the custom widgets
    void refreshGUI();  //!< Refresh the GUI on a timer

    void diveTimeStart();   //!< Start the dive timer
    void diveTimeReset();   //!< Reset the dive timer
    void diveTimeDisplay(); //!< Convert the milliseconds to minutes and hours and display in the gui
    void ledDisplay();  //!< Light up the LEDs based on each important status
    void thresholdCheck();  //!< Check the input values to see if they reach thresholds and alert the user if they do
    void loadData();    //!< Load the data from the ROV controller and display it
    void displayTime(); //!< Display the current time

private slots:
    void on_pbRelay0_clicked();

    void on_pbRelay1_clicked();

    void on_pbRelay2_clicked();

    void on_vsServo0_valueChanged(int value);

    void on_vsServo1_valueChanged(int value);

private:
    QString *version;

    Ui::MainWindow *ui;
    QActivityMonitor *activityMonitor;
    ROVAbout *dialogAbout;
    ROVDebug *dialogDebug;
    ROVMappings *dialogMappings;
    ROVSettings *dialogSettings;
    QSettings *mySettings;

    QTimer *guiTimer;
    QTime *diveTime;

    QwtPlotCurve *depthCurve;   //curve for graphing

};

#endif // MAINWINDOW_H
