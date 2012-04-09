#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    version = new QString("2.0.0 Pre-Alpha");

    guiTimer = new QTimer(this);
    guiTimer->setInterval(50); //refresh the gui 20x a second

    diveTime = new QTime();

    controller = new QROVController();

    mySettings = new QSettings("rovsuite", "monterey", this);

    setupCustomWidgets();   //load the settings for the custom widgets

    if(controller->joyAttached)
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

    guiTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showAbout()
{
    dialogAbout = new ROVAbout(this);
    dialogAbout->exec();
}

void MainWindow::showDebug()
{
    dialogDebug = new ROVDebug(this);
    dialogDebug->show();
}

void MainWindow::setupCustomWidgets()
{
    //Setup the activity monitor
    activityMonitor = new QActivityMonitor(this);
    activityMonitor->setTextEdit(ui->teLog);
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
    int depthMin = 0;
    int depthMax = 10;

    ui->plotDepth->setAxisAutoScale(2, false);  //turn off y axis auto scale
    ui->plotDepth->setAxisScale(0,-depthMax,-depthMin,1);   //set y axis scale
    ui->plotDepth->setAxisMaxMinor(0,1);    //set the y axis minor ticks
    ui->plotDepth->setAutoReplot(true); //automatically  update the plot
    ui->plotDepth->setAxisMaxMinor(2, 10);  //x axis minor ticks = 10 seconds
    ui->plotDepth->axisScaleDraw(2)->enableComponent(QwtAbstractScaleDraw::Labels, false);
    QwtText titlePlot("meters");
    titlePlot.setFont(QFont("MS Shell Dlg 2", 12));
    ui->plotDepth->setAxisTitle(QwtPlot::yLeft, titlePlot);

    //depthCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

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
}

void MainWindow::refreshGUI()
{
    controller->motherFunction();

    QTime time;
    QString timeString;
    timeString = time.currentTime().toString("hh:mm:ss ap");
    ui->labCurrentTime->setText(timeString);

    diveTimeDisplay();

    if(controller->joyAttached == true)
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

    diveTimeString.append(QString::number(hours).rightJustified(2, '0'));
    diveTimeString.append(":");
    diveTimeString.append(QString::number(minutes).rightJustified(2,'0'));
    diveTimeString.append(":");
    diveTimeString.append(QString::number(seconds).rightJustified(2, '0'));
    }
    else
    {
        diveTimeString.append("Dive time not set");
    }

    ui->labDiveTime->setText(diveTimeString);
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
