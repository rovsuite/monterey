#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    version = new QString("2.0.0 Alpha");

    guiTimer = new QTimer(this);
    guiTimer->setInterval(50); //refresh the gui 20x a second

    diveTime = new QTime();

    controller = new QROVController();

    graphTime = new QTime;
    graphTime->start();
    depthCurve = new QwtPlotCurve;
    depthCurve->setPen(QPen(Qt::darkBlue));
    depthCurve->attach(ui->plotDepth);
    graphDepthIndex = 0;
    setupCustomWidgets();   //load the settings for the custom widgets
    loadSettings();

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
    connect(guiTimer, SIGNAL(timeout()), this, SLOT(refreshGUI())); //refresh the gui
    connect(ui->actionDive_Timer_Start, SIGNAL(triggered()),this, SLOT(diveTimeStart()));
    connect(ui->actionDive_Timer_Reset, SIGNAL(triggered()), this, SLOT(diveTimeReset()));
    connect(ui->actionRescan_Joysticks, SIGNAL(triggered()), controller, SLOT(rescanJoysticks()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
    connect(ui->actionJoystick_mappings, SIGNAL(triggered()), this, SLOT(showMappings()));
    connect(controller, SIGNAL(errorTIBO()), this, SLOT(errorTIBO()));
    connect(controller, SIGNAL(errorTOBI()), this, SLOT(errorTOBI()));
    connect(controller, SIGNAL(noErrorTIBO()),this, SLOT(noErrorTIBO()));
    connect(controller, SIGNAL(noErrorTOBI()), this, SLOT(noErrorTOBI()));
    connect(controller->monitorTIBO, SIGNAL(stateChanged()), this, SLOT(lostTIBO()));
    connect(controller->monitorTOBI, SIGNAL(stateChanged()), this, SLOT(lostTOBI()));
    connect(controller, SIGNAL(savedSettings(QString)), activityMonitor, SLOT(display(QString)));
    connect(controller->monitorJoystick, SIGNAL(stateChanged()), this, SLOT(lostJoystick()));

    guiTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showAbout()
{
    // TODO: Add about information
    dialogAbout = new ROVAbout(this);
    dialogAbout->exec();
}

void MainWindow::showDebug()
{
    // TODO: Add debug code
    dialogDebug = new ROVDebug(this);
    dialogDebug->show();
}

void MainWindow::showMappings()
{
    // TODO: Add mapping code
    dialogMappings = new ROVMappings(this);
    dialogMappings->show();
}

void MainWindow::showSettings()
{
    // TODO: Finish adding settings code
    dialogSettings = new ROVSettings(this);
    connect(dialogSettings, SIGNAL(callLoadSettings()), this, SLOT(loadSettings()));    //connect the signal to load the settings
    dialogSettings->show();
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
    QString label = "Depth (";
    label.append(controller->rov->sensorDepth->getUnits());
    label.append(")");
    ui->gbDepthScale->setTitle(label);
    QwtText titlePlot(controller->rov->sensorDepth->getUnits());
    titlePlot.setFont(QFont("MS Shell Dlg 2", 12));
    ui->plotDepth->setAxisTitle(QwtPlot::yLeft, titlePlot);
    ui->labUnits0->setText(controller->rov->sensorOther0->getUnits());
    ui->labUnits1->setText(controller->rov->sensorOther1->getUnits());

    //Load the sensor names
    ui->labSensor0->setText(controller->rov->sensorOther0->getName());
    ui->labSensor1->setText(controller->rov->sensorOther1->getName());

    //Load the thresholds
    ui->scaleDepth->setMaximum(controller->rov->sensorDepth->getMax());
    ui->plotDepth->setAxisScale(0,-1.0 * controller->rov->sensorDepth->getMax(), 0,1);

    //Display loading in activity monitor
    activityMonitor->display("Settings loaded");
}

void MainWindow::setupCustomWidgets()
{
    controller->loadSettings();

    //Setup the activity monitor
    activityMonitor = new QActivityMonitor(ui->teLog, this);
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

    //Setup the needle indicator
    ui->niVoltage->setLabel("Voltage");
    ui->niVoltage->setMinValue(0);
    ui->niVoltage->setMaxValue(50);
    ui->niVoltage->setMajorTicks(6);
    ui->niVoltage->setMinorTicks(4);

    //Setup the depth plot
    ui->plotDepth->setAxisAutoScale(2, false);  //turn off y axis auto scale
    ui->plotDepth->setAxisScale(0,-controller->rov->sensorDepth->getMax(),-controller->rov->sensorDepth->getMin(),1);   //set y axis scale
    ui->plotDepth->setAxisMaxMinor(0,1);    //set the y axis minor ticks
    ui->plotDepth->setAutoReplot(true); //automatically  update the plot
    ui->plotDepth->setAxisMaxMinor(2, 10);  //x axis minor ticks = 10 seconds
    ui->plotDepth->axisScaleDraw(2)->enableComponent(QwtAbstractScaleDraw::Labels, false);
    QwtText titlePlot(controller->rov->sensorDepth->getUnits());
    titlePlot.setFont(QFont("MS Shell Dlg 2", 12));
    ui->plotDepth->setAxisTitle(QwtPlot::yLeft, titlePlot);

    depthCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

    qDebug() << "Setup the graph!";

    //Setup the compass
    QPalette *colorGroup = new QPalette(Qt::darkGray, Qt::white, Qt::lightGray, Qt::white, Qt::white, QColor::QColor(0,227,0,255), Qt::white, QColor::QColor(100,100,100,255), Qt::white);

    ui->compass->setLineWidth(5);
    ui->compass->setScaleComponents( QwtAbstractScaleDraw::Backbone |
        QwtAbstractScaleDraw::Ticks | QwtAbstractScaleDraw::Labels );
    ui->compass->setScaleTicks(0, 3, 5, 2);
    QFont compassFont = ui->compass->font();
    compassFont.setPointSize(24);
    ui->compass->setFont(compassFont);

    QMap<double, QString> map;
    map.insert(0.0, "N");
    map.insert(90.0, "E");
    map.insert(180.0, "S");
    map.insert(270.0, "W");

    ui->compass->setLabelMap(map);

    ui->compass->setPalette(*colorGroup);
    ui->compass->setNeedle(new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow, false,
         QColor::QColor(0,227,0,255)));
    ui->compass->setOrigin(270.0);
    ui->compass->setValue(0.0);
    ui->compass->setMode(QwtDial::RotateScale);

    qDebug() << "Setup the compass!";

    //Setup the QScale
    ui->scaleDepth->setMaximum(10);
    QString scaleTitle;
    scaleTitle.append("Depth (");
    scaleTitle.append(controller->rov->sensorDepth->getUnits());
    scaleTitle.append(")");
    ui->gbDepthScale->setTitle(scaleTitle);

    //Setup array of QLCDNumbers for sensor readouts
    ui->labUnitsDepth->setText(controller->rov->sensorDepth->getUnits());
    ui->labUnitsHeading->setText(controller->rov->sensorCompass->getUnits());
    ui->labUnitsVoltage->setText(controller->rov->sensorVoltage->getUnits());
    ui->labUnits0->setText(controller->rov->sensorOther0->getUnits());
    ui->labUnits1->setText(controller->rov->sensorOther1->getUnits());
}

void MainWindow::refreshGUI()
{
    controller->motherFunction();   //run the ROV control engine through its polling loop

    loadData(); //load data from the controller object
    displayTime();  //display the current time
    diveTimeDisplay();  //show the time according to the dive timer
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

void MainWindow::diveTimeStart()
{
    diveTime->start();
}

void MainWindow::diveTimeReset()
{
    diveTime->restart();
}

void MainWindow::diveTimeDisplay()
{
    QString diveTimeString;
    if(diveTime->isValid())
    {
    unsigned int hours = diveTime->elapsed() / (1000 * 60 * 60);    //convert milliseconds to hours
    unsigned int minutes = (diveTime->elapsed() % (1000 * 60 * 60)) / (1000 * 60);  //convert to minutes
    unsigned int seconds = ((diveTime->elapsed() % (1000 * 60 * 60)) % (1000*60)) / 1000;    //convert to seconds

    diveTimeString.append(QString::number(hours).rightJustified(2, '0'));   //add leading zeros
    diveTimeString.append(":");
    diveTimeString.append(QString::number(minutes).rightJustified(2,'0'));  //add leading zeros
    diveTimeString.append(":");
    diveTimeString.append(QString::number(seconds).rightJustified(2, '0')); //add leading zeros
    }
    else
    {
        diveTimeString.append("Start timer first");
    }

    ui->labDiveTime->setText(diveTimeString);
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
    // TODO: Add code to load information from QROVController
    //Display data in the numerical readouts
    ui->lcdDepth->display(controller->rov->sensorDepth->getValue());
    ui->lcdSensor0->display(controller->rov->sensorOther0->getValue());
    ui->lcdSensor1->display(controller->rov->sensorOther1->getValue());
    ui->lcdVoltage->display(controller->rov->sensorVoltage->getValue());

    //Display the data graphically
    ui->niVoltage->setValue(controller->rov->sensorVoltage->getValue());
    ui->compass->setValue(controller->rov->sensorCompass->getValue());
    ui->scaleDepth->setValue(controller->rov->sensorDepth->getValue());
    //TODO: Depth graphing
    //Graph the depth
    //Depth
    depthPoints.append(QPointF(0,0));
    depthPoints[graphDepthIndex].setX(graphTime->elapsed());
    depthPoints[graphDepthIndex].setY(-1*controller->rov->sensorDepth->getValue());
    graphDepthIndex++;
    depthCurve->setSamples(depthPoints);
    ui->plotDepth->setAxisScale(2, graphTime->elapsed()-10000, graphTime->elapsed());
    if(controller->getStatusTIBO() == false) //if ROV is not connected
    {
        depthCurve->setPen(QPen(Qt::darkGray));
    }
    else
    {
        depthCurve->setPen(QPen(Qt::darkBlue));
    }
}

void MainWindow::displayTime()
{
    QTime time;
    QString timeString;
    timeString = time.currentTime().toString("hh:mm:ss ap");
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
