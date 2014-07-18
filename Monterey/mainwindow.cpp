#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extraclasses/Fervor/fvupdater.h"
#include "extraclasses/QActivityMonitor/qactivitymonitor.h"
#include <QtWebKit>
#include <QWebView>
#include <QPalette>
#include <QMessageBox>
#include <QtDebug>
#include <QPushButton>
#include <QSpacerItem>
#include <QSlider>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include "extraclasses/QROVController/qrovcontroller.h"

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
    this->setWindowFlags(Qt::CustomizeWindowHint |
                         Qt::WindowMinMaxButtonsHint |
                         Qt::WindowCloseButtonHint);

    //UI Settings
    mySettings = new QSettings("windowSettings.ini", QSettings::IniFormat);
    loadUiGeometry();

    //GUI refresh timer
    guiTimer = new QTimer;
    guiTimer->setInterval(50); //refresh the gui 20x a second

    //Prepare status light struct
    statusLights.com = 0;
    statusLights.joystick = 0;
    statusLights.rPi = 0;
    statusLights.gear = 0;

    //ROV control engine
    bool rovControllerReady = false;
    MsgType rovControllerState = MsgType::Good;
    QString statusMessage = "";
    controller = new QROVController(rovControllerState, statusMessage);
    if(rovControllerState == MsgType::Good ||
       rovControllerState == MsgType::Warn)
    {
        rovControllerReady = true;
    }
    // Create a second thread
    engineThread = new QThread(this);

    // Move the QROVController engine to the second thread
    controller->moveToThread(engineThread);
    engineThread->start();

    // Setup the activity monitor
    activityMonitor = new QActivityMonitor(ui->teLog);
    activityMonitor->display("Monterey started...", MsgType::Info);
    QString versionDisp("Version: ");
    versionDisp.append(version);
    activityMonitor->display(versionDisp, MsgType::Info);
    activityMonitor->display(statusMessage, rovControllerState);

   if(!rovControllerReady)
   {
       qCritical() << "ROVController unable to start, QUITTING";
        exit(-2);
   }

   //Load the current logging state
   ui->actionEnable_Dive_Logging->setChecked(controller->isLoggingEnabled());

    //Add the right amount of relay buttons
    for(int i=0; i<controller->relayMappings.count(); i++)
    {
        //Add a pushbutton
        QPushButton *pb = new QPushButton(this);
        pb->setText("Relay" + QString::number(i));
        pb->setSizePolicy(QSizePolicy::Preferred,
                          QSizePolicy::MinimumExpanding);

        //Set a keyboard shortcut if it is in the range 1-9
        if(i < 9)
        {
            pb->setShortcut(Qt::Key_Control + Qt::Key_1);
        }
        pb->setCheckable(true);
        ui->groupBoxRelayButtons->layout()->addWidget(pb);
        relayButtons.append(pb);

        //Add a spacer
        QSpacerItem *spacer = new QSpacerItem(20,
                                              13,
                                              QSizePolicy::Minimum,
                                              QSizePolicy::Fixed);
        ui->groupBoxRelayButtons->layout()->addItem(spacer);

        //Link to the controller
        controller->relayMappings[i].pushButton = pb;
    }

    //Add the right amount of servo sliders
    for(int i=0; i < controller->servoMappings.count(); i++)
    {
        //Add a slider
        QSlider *slider = new QSlider(Qt::Vertical, this);
        slider->setMinimum(controller->rov().servos[i].min);
        slider->setMaximum(controller->rov().servos[i].max);
        slider->setValue(controller->rov().servos[i].defaultValue);
        ui->groupBoxServoSliders->layout()->addWidget(slider);
        servoSliders.append(slider);

        //Link to the controller
        controller->servoMappings[i].slider = slider;
    }

    //Add the right amount of sensors
    sensorDisplays.clear();
    QVBoxLayout *verticalLayout = new QVBoxLayout(ui->groupBoxSensorReadouts);
    foreach(QROVSensor sensor, controller->rov().sensors)
    {
        QHBoxLayout *hLayout = new QHBoxLayout(this);
        QLabel *labelName = new QLabel(this);
        labelName->setText(sensor.name + ":");
        labelName->setAlignment(Qt::AlignRight);
        QLCDNumber *lcd = new QLCDNumber(this);
        lcd->display(sensor.value);
        lcd->setSegmentStyle(QLCDNumber::Flat);
        sensorDisplays.append(lcd);
        QLabel *labelUnits = new QLabel(this);
        labelUnits->setText(sensor.units);
        labelUnits->setAlignment(Qt::AlignLeft);
        hLayout->addWidget(labelName);
        hLayout->addWidget(lcd);
        hLayout->addWidget(labelUnits);
        hLayout->setAlignment(hLayout, Qt::AlignHCenter);
        verticalLayout->addLayout(hLayout);
    }
    ui->groupBoxSensorReadouts->setLayout(verticalLayout);

    //Timer for updating the graph
    graphTime = new QTime;
    graphTime->start();
    ui->plotDepth->addGraph();
    ui->plotRPiCpuTempC->addGraph();
    ui->plotVoltage->addGraph();

    //Setup the video feed display
    webCamViewer = new QWebView;  //must call after load settings
    webCamViewer->setObjectName("webCamViewer");
    ui->gridLayoutHUD->addWidget(webCamViewer,1,1,4,3);

    setupCustomWidgets();   //load the settings for the custom widgets

    // Apply the event filter that captures key presses to all of the widgets
    // This allows for window-wide keyboard shortcuts that can override system
    // defaults
    QList<QWidget*> widgets = this->findChildren<QWidget*>();
    foreach(QWidget *w, widgets)
    {
        w->installEventFilter(this);
    }

    loadSettings();

    if(controller->isJoyAttached())
    {
        activityMonitor->display("Joystick attached", MsgType::Good);
        statusLights.joystick->setStatus(true);
    }
    else
    {
        activityMonitor->display("Joystick not attached", MsgType::Bad);
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
    connect(ui->actionSave_Dive_Log, SIGNAL(triggered()), this, SLOT(saveRovLogFile()));
    connect(ui->actionEnable_Dive_Logging, SIGNAL(triggered(bool)), controller, SLOT(enableLogging(bool)));
    connect(ui->actionEnable_Dive_Logging, SIGNAL(triggered(bool)), ui->actionClear_Dive_Log, SLOT(setEnabled(bool)));
    connect(ui->actionEnable_Dive_Logging, SIGNAL(triggered(bool)), ui->actionSave_Dive_Log, SLOT(setEnabled(bool)));
    connect(ui->actionClear_Dive_Log, SIGNAL(triggered()), this, SLOT(clearLog()));
    connect(controller, SIGNAL(comTiboChanged(bool)), this, SLOT(onComTiboChanged(bool)));
    connect(controller->monitorJoystick, SIGNAL(stateChanged()), this, SLOT(lostJoystick()));
    connect(controller, SIGNAL(comPiChanged(bool)), this, SLOT(onComPiChange(bool)));
    connect(controller, SIGNAL(savedSettings(QString, MsgType)), activityMonitor, SLOT(display(QString, MsgType)));
    connect(controller, SIGNAL(clickRelayButton(QPushButton*)), this, SLOT(onCalledClickRelayButton(QPushButton*)));
    connect(controller, SIGNAL(changeServo(int,int)), this, SLOT(onCalledServoChange(int,int)));
    connect(controller, SIGNAL(appendToActivityMonitor(QString, MsgType)), this, SLOT(appendToActivityMonitor(QString, MsgType)));
    connect(ui->zoomSlider, SIGNAL(sliderMoved(int)), this, SLOT(zoomTheCameraFeed(int)));
    connect(controller, SIGNAL(changedGears(int)), this, SLOT(onGearChanged(int)));

    //Connect the relay buttons to the relay handling function
    for(int i=0; i<relayButtons.count(); i++)
    {
        connect(relayButtons[i], SIGNAL(clicked()), this, SLOT(on_pbRelay_clicked()));
    }

    //Connect the servo sliders to the servo handling function
    for(int i=0; i < servoSliders.count(); i++)
    {
        connect(servoSliders[i], SIGNAL(valueChanged(int)), this, SLOT(on_vsServo_valueChanged(int)));
    }

    guiTimer->start();
    connect(controller, SIGNAL(onMotherFunctionCompleted()), this, SLOT(refreshGUI())); //refresh the GUI based on QROVController
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mySettings->beginGroup("mainWindow");
    mySettings->setValue("geometry", saveGeometry());
    mySettings->setValue("splitterStateHorizontal", ui->splitterHorizontal->saveState());
    mySettings->setValue("splitterStateVertical", ui->splitterVertical->saveState());
    mySettings->endGroup();
    event->accept();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if(ev->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEv = static_cast<QKeyEvent*>(ev);
        if(keyEv->key() >= Qt::Key_1 &&
                keyEv->key() < (Qt::Key_1 + controller->relayMappings.count()) &&
                keyEv->key() <= Qt::Key_9)
        {
            //Get the number of the key that was pressed
            int index = keyEv->key() - Qt::Key_1;
            controller->relayMappings.at(index).pushButton->click();
            return true;
        }
        else if(Qt::Key_Equal == keyEv->key())
        {
            controller->motorGearIndexIncrement();
            return true;
        }
        else if(Qt::Key_Minus == keyEv->key())
        {
            controller->motorGearIndexDecrement();
            return true;
        }
        else
        {
            return QMainWindow::eventFilter(obj, ev);
        }
    }
    else
    {
        return QMainWindow::eventFilter(obj, ev);
    }
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
    for(int i=0; i<controller->rov().relays.count(); i++)
    {
        relayButtons.at(i)->setText(controller->rov().relays[i].name);
    }

    //Display loading in activity monitor
    activityMonitor->display("Settings loaded", MsgType::Good);

    //Refresh the depth tape
    depthTape->setMaxDepth((int)controller->rov().maxDepth);

    //Load the proper video channel
    if(webCamViewer && controller->rov().videoFeed.url.isValid())
    {
        webCamViewer->load(controller->rov().videoFeed.url);
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

    //Setup status lights
    QGridLayout * statusGrid = qobject_cast<QGridLayout*>(ui->groupBoxStatus->layout());
    if(statusLights.com != 0)
    {
        delete statusLights.com;
    }
    statusLights.com = new LedIndicator;
    statusLights.com->setIndicatorTitle("COM");
    statusGrid->addWidget(statusLights.com->container, 0, 0, 1, 1);

    if(statusLights.rPi != 0)
    {
        delete statusLights.rPi;
    }
    statusLights.rPi = new LedIndicator;
    statusLights.rPi->setIndicatorTitle("RPi");
    statusGrid->addWidget(statusLights.rPi->container, 0, 1, 1, 1);

    if(statusLights.joystick != 0)
    {
        delete statusLights.joystick;
    }
    statusLights.joystick = new LedIndicator;
    statusLights.joystick->setIndicatorTitle("Joystick");
    statusGrid->addWidget(statusLights.joystick->container, 1, 0, 1, 1);

    if(statusLights.gear != 0)
    {
        delete statusLights.gear;
    }
    statusLights.gear = new LedIndicator;
    statusLights.gear->setIndicatorTitle("Disabled");
    statusGrid->addWidget(statusLights.gear->container, 1, 1, 1, 1);

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
        graphColor.setAlpha(90);
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

    QVector<QString> depthLabels;
    QVector<double> depthTicks;
    depthLabels << "0%" << "25%" << "50%" << "75%" << "max";
    depthTicks << 0 << -25 << -50 << -75 <<  -100;
    ui->plotDepth->yAxis->setAutoTickLabels(false);
    ui->plotDepth->yAxis->setAutoTicks(false);
    ui->plotDepth->yAxis->setTickVector(depthTicks);
    ui->plotDepth->yAxis->setTickVectorLabels(depthLabels);
    //ui->plotDepth->xAxis->setGrid(true);

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
    if(id >= 0 && id < servoSliders.count())
    {
        if(direction == 1)
            controller->servoMappings[id].slider->setValue(controller->servoMappings[id].slider->value() + 5);
        else
            controller->servoMappings[id].slider->setValue(controller->servoMappings[id].slider->value() - 5);
    }
    else
    {
        qDebug() << "servo ID is too large";
    }
}

void MainWindow::appendToActivityMonitor(QString message, MsgType type)
{
    if(activityMonitor)
    {
        activityMonitor->display(message, type);
    }
    else
    {
        qWarning() << "No activityMonitor object!";
    }
}

void MainWindow::saveRovLogFile()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save Dive Log", QDir::homePath(), "JSON (*.json)");
    controller->saveRovLog(filename);
}

void MainWindow::clearLog()
{
    QMessageBox msgBox;
    msgBox.setText("Clear the dive log?");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setInformativeText("All recorded data will be lost!");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Yes)
    {
        controller->clearLog();
    }
}

void MainWindow::refreshGUI()
{
    loadData(); //load data from the controller object
    displayTime();  //display the current time
    showDiveTimer();  //show the time according to the dive timer
}

void MainWindow::lostJoystick()
{
    if(controller->isJoyAttached())
    {
        activityMonitor->display("Joystick attached", MsgType::Good);
        statusLights.joystick->setStatus(true);
    }
    else
    {
        activityMonitor->display("Joystick detached", MsgType::Warn);
        statusLights.joystick->setStatus(false);
    }
}

void MainWindow::checkForUpdates()
{
    FvUpdater::sharedUpdater()->CheckForUpdatesNotSilent();
}

void MainWindow::setupDepthTape()
{
    depthTape = new DepthTape((int)controller->rov().maxDepth);
    ui->gridLayoutHUD->addWidget(depthTape->container, 1,0,4,1);
}

void MainWindow::setupCompass()
{
    compass = new Compass;
    ui->gridLayoutHUD->addWidget(compass->container, 0,0,1,4);
}

void MainWindow::loadUiGeometry()
{
    QFile settingsFile(mySettings->fileName());
    if(settingsFile.exists())
    {
        mySettings->beginGroup("mainWindow");
        restoreGeometry(mySettings->value("geometry").toByteArray());
        ui->splitterHorizontal->restoreState(mySettings->value("splitterStateHorizontal").toByteArray());
        ui->splitterVertical->restoreState(mySettings->value("splitterStateVertical").toByteArray());
        mySettings->endGroup();
    }
    else
    {
        //Default values
        ui->splitterHorizontal->setStretchFactor(0,3);
        ui->splitterHorizontal->setStretchFactor(1,6);
        ui->splitterHorizontal->setStretchFactor(2,1);

        ui->splitterVertical->setStretchFactor(0, 2);
        ui->splitterVertical->setStretchFactor(1, 1);
    }
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

void MainWindow::onComTiboChanged(bool status)
{
    if(status)
    {
        activityMonitor->display("Gained TIBO", MsgType::Good);
    }
    else
    {
        activityMonitor->display("Lost TIBO", MsgType::Bad);
    }
}

void MainWindow::onComPiChange(bool status)
{
    if(status)
    {
        activityMonitor->display("Gained Raspberry Pi COM", MsgType::Good);
    }
    else
    {
        activityMonitor->display("Lost Raspberry Pi COM", MsgType::Bad);
    }
}

void MainWindow::loadData()
{
    //Display the sensor values
    for(int i=0; i<controller->rov().sensors.count(); i++)
    {
        sensorDisplays[i]->display(controller->rov().sensors[i].value);
    }

    //Display the data graphically
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

    loadGraphData(ui->plotDepth, -100*(getDepthSensor(controller->rov()).value / controller->rov().maxDepth), false, true);
    loadGraphData(ui->plotRPiCpuTempC, controller->rov().piData.tempC, true, false);
    loadGraphData(ui->plotVoltage, getVoltageSensor(controller->rov()).value, true, false);

    depthTape->onDepthChange(getDepthSensor(controller->rov()).value, getDepthSensor(controller->rov()).units);   //TODO: test
    compass->onHeadingChange(getHeadingSensor(controller->rov()).value);

    //Light up the indicators
    if(controller->getStatusTIBO() != statusLights.com->status())
        statusLights.com->setStatus(!statusLights.com->status());

    if(controller->getStatusPi() != statusLights.rPi->status())
        statusLights.rPi->setStatus(controller->getStatusPi());
}

void MainWindow::displayTime()
{
    QTime time;
    QString timeString;
    timeString = time.currentTime().toString("hh:mm:ss");
    ui->labCurrentTime->setText(timeString);
}

//Handle UI QPushButton clicks for relays
void MainWindow::on_pbRelay_clicked()
{
    for(int i=0; i<controller->relayMappings.count(); i++)
    {
        if(controller->relayMappings[i].pushButton == sender())
        {
            controller->editRov().relays[i].enabled = relayButtons.at(i)->isChecked();
        }
    }
}

//Handle UI QSlider change events for servos
void MainWindow::on_vsServo_valueChanged(int value)
{
    for(int i=0; i < controller->servoMappings.count(); i++)
    {
        if(controller->servoMappings[i].slider == sender())
        {
            controller->editRov().servos[i].value = value;
        }
    }
}

// Copies log to system clipboard for easy sharing
void MainWindow::on_buttonCopyLogToClipboard_clicked()
{
    QClipboard *p_Clipboard = QApplication::clipboard();
    p_Clipboard->setText(ui->teLog->toPlainText());
}

// Deletes all log information up to the point when this is clicked
void MainWindow::on_buttonClearLog_clicked()
{
    ui->teLog->clear();
}

void MainWindow::onGearChanged(int gear)
{
    if(statusLights.gear != 0)
    {
        QString title = (0 == gear ?
                        "Disabled" : ("Gear "+ QString::number(gear)));
        statusLights.gear->setIndicatorTitle(title);
        statusLights.gear->setStatus(gear != 0);
    }
}
