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
#include <QPalette>
#include <QMessageBox>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Setup the base of the application
    ui->setupUi(this);
    version = new QString(QApplication::applicationVersion());
    QString title = this->windowTitle();
    title.append(" ");
    title.append(version);
    this->setWindowTitle(title);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    //GUI refresh timer
    guiTimer = new QTimer;
    guiTimer->setInterval(50); //refresh the gui 20x a second

    //Prepare status light struct
    statusLights.com = 0;
    statusLights.joystick = 0;
    statusLights.rPi = 0;
    statusLights.tahoe = 0;

    //ROV control engine
    controller = new QROVController();
    engineThread = new QThread(this); //create a second thread
    controller->moveToThread(engineThread); //move the QROVController engine to the second thread
    engineThread->start();

    controller->rov->listRelays[0]->setQPushButton(ui->pbRelay0);
    controller->rov->listRelays[1]->setQPushButton(ui->pbRelay1);
    controller->rov->listRelays[2]->setQPushButton(ui->pbRelay2);

    //Timer for updating the graph
    graphTime = new QTime;
    graphTime->start();
    ui->plotDepth->addGraph();
    ui->plotRPiCpuTempC->addGraph();
    ui->plotSensor0->addGraph();
    ui->plotVoltage->addGraph();

    //Setup the video feed display
    webCamViewer = new QWebView;  //must call after load settings
    webCamViewer->setObjectName("webCamViewer");
    ui->gridLayoutHUD->addWidget(webCamViewer,1,1,4,3);

    setupCustomWidgets();   //load the settings for the custom widgets

    loadSettings();

    if(controller->isJoyAttached())
    {
        activityMonitor->display("Joystick attached");
        statusLights.joystick->setStatus(true);
    }
    else
    {
        activityMonitor->display("Joystick not attached");
        statusLights.joystick->setStatus(false);
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
    connect(controller->monitorTIBO, SIGNAL(stateChanged()), this, SLOT(lostTIBO()));
    connect(controller->monitorTOBI, SIGNAL(stateChanged()), this, SLOT(lostTOBI()));
    connect(controller->monitorJoystick, SIGNAL(stateChanged()), this, SLOT(lostJoystick()));
    connect(controller->monitorTahoe, SIGNAL(stateChanged()), this, SLOT(lostTahoe()));
    connect(controller->monitorRPi, SIGNAL(stateChanged()), this, SLOT(lostRPi()));
    connect(controller, SIGNAL(savedSettings(QString)), activityMonitor, SLOT(display(QString)));
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
    dialogAbout->show();
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
    ui->labUnits0->setText(controller->rov->sensorOther0->getUnits());
    ui->labUnits1->setText(controller->rov->sensorOther1->getUnits());

    //Load the sensor names
    ui->labSensor0->setText(controller->rov->sensorOther0->getName());
    ui->labSensor1->setText(controller->rov->sensorOther1->getName());
    QCPPlotTitle *title = 0;
    title = qobject_cast<QCPPlotTitle*>(ui->plotSensor0->plotLayout()->element(0,0));
    if(title != 0)
    {
        QCPPlotTitle *newTitle = new QCPPlotTitle(ui->plotSensor0);
        newTitle->setText(controller->rov->sensorOther0->getName());
        newTitle->setFont(title->font());
        newTitle->setTextColor(title->textColor());
        ui->plotSensor0->plotLayout()->remove(title);
        ui->plotSensor0->plotLayout()->addElement(0,0,newTitle);
        ui->plotSensor0->replot();  //refreshes the title
    }
    else
        qWarning() << "No QCPPlotTitle on " << ui->plotSensor0;


    //Display loading in activity monitor
    activityMonitor->display("Settings loaded");

    //Refresh the depth tape
    depthTape->setMaxDepth((int)controller->rov->sensorDepth->getMax());

    qDebug() << "IP Video URL: " << controller->rov->getVideoFeeds().first()->url();
    //Load the proper video channel
    if(webCamViewer && controller->rov->getVideoFeeds().first()->url().isValid())
    {
        webCamViewer->load(controller->rov->getVideoFeeds().first()->url());
        webCamViewer->show();
    }
    else
    {
        QMessageBox errorMessageBox;
        errorMessageBox.setText("Error setting the video feed URL.");
        errorMessageBox.setInformativeText("The URL is invalid.  Please change it in the settings!");
        errorMessageBox.setStandardButtons(QMessageBox::Ok);
        errorMessageBox.setDefaultButton(QMessageBox::Ok);
        errorMessageBox.exec();
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

    //Setup status lights
    QGridLayout * statusGrid = qobject_cast<QGridLayout*>(ui->groupBoxStatus->layout());
    if(statusLights.com != 0)
    {
        delete statusLights.com;
    }
    statusLights.com = new LedIndicator;
    statusLights.com->setIndicatorTitle("COM");
    statusLights.com->setStatus(false);
    statusGrid->addWidget(statusLights.com->container, 0, 0, 1, 1);

    if(statusLights.rPi != 0)
    {
        delete statusLights.rPi;
    }
    statusLights.rPi = new LedIndicator;
    statusLights.rPi->setIndicatorTitle("RPi");
    statusLights.rPi->setStatus(false);
    statusGrid->addWidget(statusLights.rPi->container, 0, 1, 1, 1);

    if(statusLights.joystick != 0)
    {
        delete statusLights.joystick;
    }
    statusLights.joystick = new LedIndicator;
    statusLights.joystick->setIndicatorTitle("Joystick");
    statusLights.joystick->setStatus(false);
    statusGrid->addWidget(statusLights.joystick->container, 1, 0, 1, 1);

    if(statusLights.tahoe != 0)
    {
        delete statusLights.tahoe;
    }
    statusLights.tahoe = new LedIndicator;
    statusLights.tahoe->setIndicatorTitle("Tahoe");
    statusLights.tahoe->setStatus(false);
    statusGrid->addWidget(statusLights.tahoe->container, 1, 1, 1, 1);

    //Setup the plots
    auto setStyleOnPlot = [this]( QCustomPlot *plot, QString titleString )
    {
        //Colors
        plot->yAxis->setTickLabelColor(this->palette().windowText().color());
        plot->yAxis->setTickPen(QPen(this->palette().windowText().color()));
        plot->yAxis->setSubTickPen(QPen(this->palette().windowText().color()));
        plot->yAxis->setBasePen(QPen(this->palette().windowText().color()));
        plot->xAxis->setTickLabelColor(this->palette().windowText().color());
        plot->xAxis->setTickPen(QPen(this->palette().windowText().color()));
        plot->xAxis->setSubTickPen(QPen(this->palette().windowText().color()));
        plot->xAxis->setBasePen(QPen(this->palette().windowText().color()));
        plot->setBackground(QBrush(this->palette().window().color()));

        plot->yAxis2->setTickLabelColor(this->palette().windowText().color());
        plot->yAxis2->setTickPen(QPen(this->palette().windowText().color()));
        plot->yAxis2->setSubTickPen(QPen(this->palette().windowText().color()));
        plot->yAxis2->setBasePen(QPen(this->palette().windowText().color()));
        plot->xAxis2->setTickLabelColor(this->palette().windowText().color());
        plot->xAxis2->setTickPen(QPen(this->palette().windowText().color()));
        plot->xAxis2->setSubTickPen(QPen(this->palette().windowText().color()));
        plot->xAxis2->setBasePen(QPen(this->palette().windowText().color()));

        plot->graph(0)->setPen(QPen(this->palette().windowText().color()));
        QColor graphColor = this->palette().highlight().color();
        graphColor.setAlpha(128);
        plot->graph(0)->setBrush(QBrush(graphColor));

        //Title
        plot->plotLayout()->insertRow(0);
        QCPPlotTitle *title = new QCPPlotTitle(plot);
        title->setText(titleString);
        title->setTextColor(this->palette().windowText().color());
        QFont font;
        font.setFamily(font.defaultFamily());
        font.setPointSize(12);
        font.setBold(true);
        plot->setFont(font);
        plot->plotLayout()->addElement(0, 0, title);

        //xAxis
        plot->xAxis->setTickStep(1000);    //set to 1000ms gaps
        plot->xAxis->setTickLabels(false); //hide labels
        plot->xAxis->grid()->setVisible(false);

        //yAxis
        plot->yAxis->setAutoTickCount(3);

        //Show axes on all edges
        plot->axisRect()->setupFullAxesBox();
        connect(plot->yAxis, SIGNAL(rangeChanged(QCPRange)), plot->yAxis2, SLOT(setRange(QCPRange)));
        connect(plot->xAxis, SIGNAL(rangeChanged(QCPRange)), plot->xAxis2, SLOT(setRange(QCPRange)));
    };

    //Depth Plot
    ui->plotDepth->yAxis->setRange(0,-100);
    setStyleOnPlot(ui->plotDepth, "Depth %");

    //Voltage Plot
    setStyleOnPlot(ui->plotVoltage, "Voltage");

    //Raspberry Pi CPU temperature C Plot
    QString rPiTitle = "RPi CPU Temp ";
    rPiTitle.append(QChar(0x00B0));
    rPiTitle.append("C");
    setStyleOnPlot(ui->plotRPiCpuTempC, rPiTitle);

    //Sensor0 graph
    setStyleOnPlot(ui->plotSensor0, controller->rov->sensorOther0->getName());

    QVector<QString> depthLabels;
    QVector<double> depthTicks;
    depthLabels << "0%" << "25%" << "50%" << "75%" << "max";
    depthTicks << 0 << -25 << -50 << -75 <<  -100;
    ui->plotDepth->yAxis->setAutoTickLabels(false);
    ui->plotDepth->yAxis->setAutoTicks(false);
    ui->plotDepth->yAxis->setTickVector(depthTicks);
    ui->plotDepth->yAxis->setTickVectorLabels(depthLabels);
    //ui->plotDepth->xAxis->setGrid(true);

    //Setup array of QLCDNumbers for sensor readouts
    ui->labUnitsDepth->setText(controller->rov->sensorDepth->getUnits());
    ui->labUnitsHeading->setText(controller->rov->sensorCompass->getUnits());
    ui->labUnitsVoltage->setText(controller->rov->sensorVoltage->getUnits());
    ui->labUnits0->setText(controller->rov->sensorOther0->getUnits());
    ui->labUnits1->setText(controller->rov->sensorOther1->getUnits());

    setupDepthTape();
    setupCompass();
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
    thresholdCheck();   //check for values exceeding thresholds
}

void MainWindow::lostJoystick()
{
    if(controller->isJoyAttached())
    {
        activityMonitor->display("Joystick attached");
        statusLights.joystick->setStatus(true);
    }
    else
    {
        activityMonitor->display("Joystick detached");
        statusLights.joystick->setStatus(false);
    }
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
    ui->gridLayoutHUD->addWidget(depthTape->container, 1,0,4,1);
}

void MainWindow::setupCompass()
{
    compass = new Compass;
    ui->gridLayoutHUD->addWidget(compass->container, 0,0,1,4);
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

void MainWindow::lostTOBI()
{
    if(controller->getStatusTOBI())
    {
        activityMonitor->display("Gained TOBI");
        if(controller->getStatusTIBO()) //if also receiving
            statusLights.com->setStatus(true);
    }
    else
    {
        activityMonitor->display("Lost TOBI");
        statusLights.com->setStatus(false);
    }
}

void MainWindow::lostTIBO()
{
    if(controller->getStatusTIBO())
    {
        activityMonitor->display("Gained TIBO");
        if(controller->getStatusTOBI()) //if also sending
            statusLights.com->setStatus(true);
    }
    else
    {
        activityMonitor->display("Lost TIBO");
        statusLights.com->setStatus(false);
    }
}

void MainWindow::lostTahoe()
{
    if(controller->getStatusTahoe())
    {
        activityMonitor->display("Gained Tahoe COM");
        statusLights.tahoe->setStatus(true);
    }
    else
    {
        activityMonitor->display("Lost Tahoe COM");
        statusLights.tahoe->setStatus(false);
    }
}

void MainWindow::lostRPi()
{
    if(controller->getStatusPi())
    {
        activityMonitor->display("Gained Raspberry Pi COM");
        statusLights.rPi->setStatus(true);
    }
    else
    {
        activityMonitor->display("Lost Raspberry Pi COM");
        statusLights.rPi->setStatus(false);
    }
}

void MainWindow::thresholdCheck()   //TODO: REMOVE
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
    depthPoints.append(-100*(controller->rov->sensorDepth->getValue()/controller->rov->sensorDepth->getMax()));
    voltagePoints.append(controller->rov->sensorVoltage->getValue());
    rPiCpuTempCPoints.append(controller->rov->piData->tempC());
    sensor0Points.append(controller->rov->sensorOther0->getValue());

    int timeElapsed = graphTime->elapsed();

    auto loadGraphData = [this, timeElapsed]( QCustomPlot *plot, double dataPoint, bool autoAdjustYAxis, bool canBeNegative)
    {
        plot->graph(0)->addData(timeElapsed, dataPoint);
        plot->xAxis->setRangeUpper(timeElapsed);
        plot->xAxis->setRangeLower(timeElapsed - 30000);

        //Remove no longer visible data
        plot->graph(0)->removeDataBefore(timeElapsed - 30000);

        if(autoAdjustYAxis)
        {
            plot->yAxis->setRangeUpper((int)dataPoint + 10);

            int lower = (int)dataPoint - 10;
            if(!canBeNegative && lower < 0)
            {
                plot->yAxis->setRangeLower(0);
            }
            else
            {
                plot->yAxis->setRangeLower(lower);
            }
        }
        plot->replot();
    };

    loadGraphData(ui->plotDepth, depthPoints.last(), false, true);
    loadGraphData(ui->plotRPiCpuTempC, rPiCpuTempCPoints.last(), true, false);
    loadGraphData(ui->plotSensor0, sensor0Points.last(), true, true);
    loadGraphData(ui->plotVoltage, voltagePoints.last(), true, false);

    depthTape->onDepthChange(controller->rov->sensorDepth->getValue(), controller->rov->sensorDepth->getUnits());
    compass->onHeadingChange(controller->rov->sensorCompass->getValue());
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
