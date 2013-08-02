/*
    Copyright (C) 2012  Chris Konstad (chriskon149@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extraclasses/Fervor/fvupdater.h"
#include <QtWebKit>
#include <QWebView>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    version = new QString(QApplication::applicationVersion());
    QString title = this->windowTitle();
    title.append(" ");
    title.append(version);
    this->setWindowTitle(title);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    /*
    //Set the new font
    QFont font = this->font();
    //font.setFamily("Alfphabet");
    QApplication::setFont(font);
    QPalette palette = this->palette();
    QBrush brush = palette.brush(QPalette::Window);
    brush.setColor(QColor("#000000"));
    palette.setBrush(QPalette::Window, brush);
    brush = palette.brush(QPalette::WindowText);
    brush.setColor(QColor("#bcd5fe"));
    palette.setBrush(QPalette::WindowText, brush);

    QList<QObject*> widgets = this->findChildren<QObject*>();
    foreach(QObject* widget, widgets)
    {
        QWidget* widgetPointer = qobject_cast<QGroupBox*>(widget);
        if(widgetPointer)
        {
            widgetPointer->setPalette(palette);
        }
        QGroupBox* groupBox = qobject_cast<QGroupBox*>(widget);
        if(groupBox)
        {
            groupBox->setStyleSheet("QGroupBox { border: 2px solid #0275ac }");
        }
    }
    this->setPalette(palette);
    */


    guiTimer = new QTimer;
    guiTimer->setInterval(50); //refresh the gui 20x a second

    controller = new QROVController();
    engineThread = new QThread(this); //create a second thread
    controller->moveToThread(engineThread); //move the QROVController engine to the second thread
    engineThread->start();

    controller->rov->listRelays[0]->setQPushButton(ui->pbRelay0);
    controller->rov->listRelays[1]->setQPushButton(ui->pbRelay1);
    controller->rov->listRelays[2]->setQPushButton(ui->pbRelay2);

    graphTime = new QTime;
    graphTime->start();
    ui->plotDepth->addGraph();
    ui->plotDepth->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 100)));
    setupCustomWidgets();   //load the settings for the custom widgets
    loadSettings();


    webCamViewer = new QWebView;  //must call after load settings
    webCamViewer->setObjectName("webCamViewer");
    ui->gridLayoutHUD->addWidget(webCamViewer,0,1,4,4);
    webCamViewer->load(controller->rov->getVideoFeeds().at(0)->url());

    if(controller->isJoyAttached())
    {
        activityMonitor->display("Joystick attached");
    }
    else
    {
        activityMonitor->display("Joystick not attached");
    }

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout())); //show the about window
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));  //exit the application
    connect(ui->actionDebug, SIGNAL(triggered()), this, SLOT(showDebug())); //show the debug window
    connect(ui->actionDive_Timer_Reset, SIGNAL(triggered()), this->controller, SLOT(diveTimeReset()));
    connect(ui->actionRescan_Joysticks, SIGNAL(triggered()), controller, SLOT(rescanJoysticks()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
    connect(ui->actionJoystick_mappings, SIGNAL(triggered()), this, SLOT(showMappings()));
    connect(ui->actionCheck_for_Updates, SIGNAL(triggered()), this, SLOT(checkForUpdates()));
    connect(ui->actionFullscreen, SIGNAL(toggled(bool)), this, SLOT(showFullscreen(bool)));
    connect(controller, SIGNAL(errorTIBO()), this, SLOT(errorTIBO()));
    connect(controller, SIGNAL(errorTOBI()), this, SLOT(errorTOBI()));
    connect(controller, SIGNAL(noErrorTIBO()),this, SLOT(noErrorTIBO()));
    connect(controller, SIGNAL(noErrorTOBI()), this, SLOT(noErrorTOBI()));
    connect(controller->monitorTIBO, SIGNAL(stateChanged()), this, SLOT(lostTIBO()));
    connect(controller->monitorTOBI, SIGNAL(stateChanged()), this, SLOT(lostTOBI()));
    connect(controller, SIGNAL(savedSettings(QString)), activityMonitor, SLOT(display(QString)));
    connect(controller->monitorJoystick, SIGNAL(stateChanged()), this, SLOT(lostJoystick()));
    connect(controller, SIGNAL(onTahoeProcessed()), this, SLOT(displayTahoe()));
    connect(controller, SIGNAL(clickRelayButton(QPushButton*)), this, SLOT(onCalledClickRelayButton(QPushButton*)));
    connect(controller, SIGNAL(changeServo(int,int)), this, SLOT(onCalledServoChange(int,int)));
    connect(controller, SIGNAL(appendToActivityMonitor(QString)), this, SLOT(appendToActivityMonitor(QString)));
    connect(ui->zoomSlider, SIGNAL(sliderMoved(int)), this, SLOT(zoomTheCameraFeed(int)));

    guiTimer->start();
    connect(controller, SIGNAL(onMotherFunctionCompleted()), this, SLOT(refreshGUI())); //refresh the GUI based on QROVController

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showAbout()
{
    dialogAbout = new ROVAbout(this);
    dialogAbout->setAttribute(Qt::WA_DeleteOnClose);
    dialogAbout->exec();
}

void MainWindow::showDebug()
{
    dialogDebug = new ROVDebug(this);
    dialogDebug->setAttribute(Qt::WA_DeleteOnClose);
    dialogDebug->show();
}

void MainWindow::showMappings()
{
    dialogMappings = new ROVMappings(this);
    dialogMappings->setAttribute(Qt::WA_DeleteOnClose);
    dialogMappings->show();
}

void MainWindow::showSettings()
{
    dialogSettings = new ROVSettings(this);
    dialogSettings->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialogSettings, SIGNAL(callLoadSettings()), this, SLOT(loadSettings()));    //connect the signal to load the settings
    dialogSettings->show();
}

void MainWindow::showDiveTimer()
{
    ui->labDiveTime->setText(controller->diveTimeString());
}

void MainWindow::loadSettings()
{
    //Load the settings into the controller
    controller->loadSettings();

    //Load the relay names
    ui->pbRelay0->setText(controller->rov->listRelays[0]->getName());
    ui->pbRelay1->setText(controller->rov->listRelays[1]->getName());
    ui->pbRelay2->setText(controller->rov->listRelays[2]->getName());

    //Load the units
    ui->labUnitsDepth->setText(controller->rov->sensorDepth->getUnits());
    QString depthTitle = "Depth (";
    depthTitle.append(controller->rov->sensorDepth->getUnits());
    depthTitle.append(")");
    ui->plotDepth->setTitle(depthTitle);
    ui->labUnits0->setText(controller->rov->sensorOther0->getUnits());
    ui->labUnits1->setText(controller->rov->sensorOther1->getUnits());

    //Load the sensor names
    ui->labSensor0->setText(controller->rov->sensorOther0->getName());
    ui->labSensor1->setText(controller->rov->sensorOther1->getName());

    //Load the thresholds
    ui->plotDepth->yAxis->setRange(0,-1*controller->rov->sensorDepth->getMax());
    ui->plotDepth->replot();    //repaint the element to update the title and range

    //Display loading in activity monitor
    activityMonitor->display("Settings loaded");

    //Refresh the depth tape
    depthTape->setMaxDepth((int)controller->rov->sensorDepth->getMax());

    //Load the proper video channel
    if(webCamViewer)
    {
        webCamViewer->setUrl(controller->rov->getVideoFeeds().at(0)->url());
    }
}

void MainWindow::setupCustomWidgets()
{
    controller->loadSettings();

    //Setup the activity monitor
    activityMonitor = new QActivityMonitor(ui->teLog);
    activityMonitor->display("Monterey started...");
    QString versionDisp("Version: ");
    versionDisp.append(version);
    activityMonitor->display(versionDisp);

    //Setup the LEDs
    QList<QLedIndicator*> red;
    red.append(ui->ledErrorR);
    red.append(ui->ledJoystickR);
    red.append(ui->ledTIBOR);
    red.append(ui->ledTOBIR);
    red.append(ui->ledVoltageR);

    foreach(QLedIndicator* led, red)
    {
        led->setOnColor1(QColor(255,0,0));
        led->setOnColor2(QColor(192,0,0));
        led->setOffColor1(QColor(28,0,0));
        led->setOffColor2(QColor(128,0,0));
    }

    QList<QLedIndicator*> yellow;
    yellow.append(ui->ledErrorY);
    yellow.append(ui->ledJoystickY);
    yellow.append(ui->ledTIBOY);
    yellow.append(ui->ledTOBIY);
    yellow.append(ui->ledVoltageY);

    foreach(QLedIndicator* led, yellow)
    {
        led->setOnColor1(QColor(255,255,0));
        led->setOnColor2(QColor(192,192,0));
        led->setOffColor1(QColor(28,28,0));
        led->setOffColor2(QColor(128,128,0));
    }

    //Setup the depth plot
    QString depthTitle = "Depth (";
    depthTitle.append(controller->rov->sensorDepth->getUnits());
    depthTitle.append(")");
    ui->plotDepth->setTitle(depthTitle);
    ui->plotDepth->yAxis->setRange(0,-1*controller->rov->sensorDepth->getMax());
    ui->plotDepth->xAxis->setTickStep(1000);    //set to 1000ms gaps
    ui->plotDepth->xAxis->setTickLabels(false); //hide labels
    ui->plotDepth->setColor(this->palette().background().color());
    qDebug() << "Setup the graph!" << ui->plotDepth;

    //Setup array of QLCDNumbers for sensor readouts
    ui->labUnitsDepth->setText(controller->rov->sensorDepth->getUnits());
    ui->labUnitsHeading->setText(controller->rov->sensorCompass->getUnits());
    ui->labUnitsVoltage->setText(controller->rov->sensorVoltage->getUnits());
    ui->labUnits0->setText(controller->rov->sensorOther0->getUnits());
    ui->labUnits1->setText(controller->rov->sensorOther1->getUnits());

    setupDepthTape();
}

void MainWindow::onCalledClickRelayButton(QPushButton *button)
{
    Q_ASSERT(button != 0);
    button->click();
}

void MainWindow::onCalledServoChange(int id, int direction)
{
    if(id == 0)
    {
        if(direction == 1)
            ui->vsServo0->setValue(ui->vsServo0->value() + 5);
        else
            ui->vsServo0->setValue(ui->vsServo0->value() - 5);
    }
    else if(id == 1)
    {
        if(direction == 1)
            ui->vsServo1->setValue(ui->vsServo1->value() + 5);
        else
            ui->vsServo1->setValue(ui->vsServo1->value() - 5);
    }
    else
    {
        qDebug() << "servo ID is too large";
    }
}

void MainWindow::appendToActivityMonitor(QString message)
{
    if(activityMonitor)
    {
        activityMonitor->display(message);
    }
    else
    {
        qWarning() << "No activityMonitor object!";
    }
}

void MainWindow::refreshGUI()
{
    loadData(); //load data from the controller object
    displayTime();  //display the current time
    showDiveTimer();  //show the time according to the dive timer
    ledDisplay();   //light up the LEDs
    thresholdCheck();   //check for values exceeding thresholds
}

void MainWindow::lostJoystick()
{
    if(controller->isJoyAttached())
        activityMonitor->display("Joystick attached");
    else
        activityMonitor->display("Joystick detached");
}

void MainWindow::displayTahoe()
{
    ui->pbRelay0->setChecked(controller->rov->listRelays[0]->getState());
    ui->pbRelay1->setChecked(controller->rov->listRelays[1]->getState());
    ui->pbRelay2->setChecked(controller->rov->listRelays[2]->getState());

    ui->vsServo0->setValue(controller->rov->listServos[0]->getValue());
    ui->vsServo1->setValue(controller->rov->listServos[1]->getValue());
}

void MainWindow::checkForUpdates()
{
    FvUpdater::sharedUpdater()->CheckForUpdatesNotSilent();
}

void MainWindow::setupDepthTape()
{
    depthTape = new DepthTape((int)controller->rov->sensorDepth->getMax());
    ui->gridLayoutHUD->addWidget(depthTape->container, 0,0,4,1);
}

void MainWindow::showFullscreen(bool fullscreen)
{
    if(fullscreen)
        this->showFullScreen();
    else
        this->showNormal();
}

void MainWindow::zoomTheCameraFeed(int zoomAmount)
{
    qreal zoom = (qreal)zoomAmount/100.0;
    if(webCamViewer)
    {
        webCamViewer->setZoomFactor(zoom);
    }
}

void MainWindow::ledDisplay()
{
    //Joystick
    if(controller->isJoyAttached() == true)
    {
        ui->ledJoystickG->setChecked(true);
        ui->ledJoystickY->setChecked(false);
        ui->ledJoystickR->setChecked(false);
    }
    else
    {
        ui->ledJoystickG->setChecked(false);
        ui->ledJoystickY->setChecked(false);
        ui->ledJoystickR->setChecked(true);
    }

    //Voltage
    if(controller->rov->sensorVoltage->getValue() >= controller->rov->sensorVoltage->getMax())
    {
        ui->ledVoltageG->setChecked(false);
        ui->ledVoltageR->setChecked(false);
        ui->ledVoltageY->setChecked(true);
    }
    else if(controller->rov->sensorVoltage->getValue() < 1)
    {
        ui->ledVoltageG->setChecked(false);
        ui->ledVoltageR->setChecked(true);
        ui->ledVoltageY->setChecked(false);
    }
    else
    {
        ui->ledVoltageG->setChecked(true);
        ui->ledVoltageR->setChecked(false);
        ui->ledVoltageY->setChecked(false);
    }

    //Tahoe LED
    if(controller->getStatusTahoe())    //if Tahoe is connected
    {
        ui->ledTahoe->setChecked(true);
    }
    else
    {
        ui->ledTahoe->setChecked(false);
    }

    //Error LED
    if(ui->ledJoystickR->isChecked() || ui->ledTIBOR->isChecked() || ui->ledTOBIR->isChecked() || ui->ledVoltageR->isChecked())
    {
        ui->ledErrorG->setChecked(false);
        ui->ledErrorR->setChecked(true);
        ui->ledErrorY->setChecked(false);
    }
    else if(ui->ledJoystickY->isChecked() || ui->ledTIBOY->isChecked() || ui->ledTOBIY->isChecked() || ui->ledVoltageY->isChecked())
    {
        ui->ledErrorG->setChecked(false);
        ui->ledErrorR->setChecked(false);
        ui->ledErrorY->setChecked(true);
    }
    else
    {
        ui->ledErrorG->setChecked(true);
        ui->ledErrorR->setChecked(false);
        ui->ledErrorY->setChecked(false);
    }
}

void MainWindow::noErrorTOBI()
{
    ui->ledTOBIG->setChecked(true);
    ui->ledTOBIR->setChecked(false);
    ui->ledTOBIY->setChecked(false);
}

void MainWindow::noErrorTIBO()
{
    ui->ledTIBOG->setChecked(true);
    ui->ledTIBOR->setChecked(false);
    ui->ledTIBOY->setChecked(false);
}

void MainWindow::errorTOBI()
{
    ui->ledTOBIG->setChecked(false);
    ui->ledTOBIR->setChecked(true);
    ui->ledTOBIY->setChecked(false);
}

void MainWindow::errorTIBO()
{
    ui->ledTIBOG->setChecked(false);
    ui->ledTIBOR->setChecked(true);
    ui->ledTIBOY->setChecked(false);
}

void MainWindow::lostTOBI()
{
    if(controller->getStatusTOBI())
        activityMonitor->display("Gained TOBI");
    else
        activityMonitor->display("Lost TOBI");
}

void MainWindow::lostTIBO()
{
    if(controller->getStatusTIBO())
        activityMonitor->display("Gained TIBO");
    else
        activityMonitor->display("Lost TIBO");
}

void MainWindow::thresholdCheck()
{
    //if the ROV is too deep
    if(controller->rov->sensorDepth->getValue() > controller->rov->sensorDepth->getThreshold())
    {
        ui->lcdDepth->setStyleSheet("QLCDNumber { color : red }");
    }
    else    //if ROV's depth is OK
    {
        ui->lcdDepth->setStyleSheet("QLCDNumber { color : none }");
    }
}

void MainWindow::loadData()
{
    //Display data in the numerical readouts
    ui->lcdDepth->display(controller->rov->sensorDepth->getValue());
    ui->lcdSensor0->display(controller->rov->sensorOther0->getValue());
    ui->lcdSensor1->display(controller->rov->sensorOther1->getValue());
    ui->lcdVoltage->display(controller->rov->sensorVoltage->getValue());
    ui->lcdHeading->display(controller->rov->sensorCompass->getValue());

    //Display the data graphically
    //Graph the depth
    //Depth
    depthPoints.append(-1*controller->rov->sensorDepth->getValue());
    seconds.append(graphTime->elapsed());
    ui->plotDepth->graph(0)->setData(seconds, depthPoints);
    ui->plotDepth->xAxis->setRangeUpper(graphTime->elapsed());
    ui->plotDepth->xAxis->setRangeLower(graphTime->elapsed() - 10000);
    if(controller->getStatusTIBO() == false) //if ROV is not connected
    {
        ui->plotDepth->graph(0)->setPen(QPen(Qt::darkGray));
        ui->plotDepth->graph(0)->setBrush(QBrush(QColor(128, 128, 128, 100)));
    }
    else
    {
        ui->plotDepth->graph(0)->setPen(QPen(Qt::blue));
        ui->plotDepth->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 100)));
    }
    ui->plotDepth->replot();
    depthTape->onDepthChange(controller->rov->sensorDepth->getValue(), controller->rov->sensorDepth->getUnits());
}

void MainWindow::displayTime()
{
    QTime time;
    QString timeString;
    timeString = time.currentTime().toString("hh:mm:ss");
    ui->labCurrentTime->setText(timeString);
}

void MainWindow::on_pbRelay0_clicked()
{
    controller->rov->listRelays[0]->setState(ui->pbRelay0->isChecked());
}

void MainWindow::on_pbRelay1_clicked()
{
    controller->rov->listRelays[1]->setState(ui->pbRelay1->isChecked());
}

void MainWindow::on_pbRelay2_clicked()
{
    controller->rov->listRelays[2]->setState(ui->pbRelay2->isChecked());
}

void MainWindow::on_vsServo0_valueChanged(int value)
{
    controller->rov->listServos[0]->setValue(value);
}

void MainWindow::on_vsServo1_valueChanged(int value)
{
    controller->rov->listServos[1]->setValue(value);
}

// Copies log to system clipboard for easy sharing
void MainWindow::on_buttonCopyLogToClipboard_clicked()
{
//    This is more of a hack-ish solution
//    ui->teLog->selectAll();
//    ui->teLog->copy();

    QClipboard *p_Clipboard = QApplication::clipboard();
    p_Clipboard->setText(ui->teLog->toPlainText());
}

// Deletes all log information up to the point when this is clicked
void MainWindow::on_buttonClearLog_clicked()
{
    ui->teLog->clear();
}
