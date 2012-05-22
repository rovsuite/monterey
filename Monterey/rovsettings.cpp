#include "rovsettings.h"
#include "ui_rovsettings.h"

ROVSettings::ROVSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVSettings)
{
    //TODO: Add ability to change which ports Monterey listens on?
    ui->setupUi(this);
    //mySettings = new QSettings("rovsuite", "monterey", this);
    mySettings = new QSettings("settings.ini", QSettings::IniFormat);

    //Load settings
    ui->leNameRelay0->setText(mySettings->value("names/relay0", "relay1").toString());
    ui->leNameRelay1->setText(mySettings->value("names/relay1", "relay2").toString());
    ui->leNameRelay2->setText(mySettings->value("names/relay2", "relay3").toString());

    ui->leUnitsDepth->setText(mySettings->value("units/depth", "meters").toString());
    ui->leUnitsSensor0->setText(mySettings->value("units/sensor0", "units").toString());
    ui->leUnitsSensor1->setText(mySettings->value("units/sensor1", "units").toString());

    ui->sbDepth->setValue(mySettings->value("thresholds/depth", "4").toDouble());
    ui->sbVoltage->setValue(mySettings->value("thresholds/voltage", "11").toDouble());

    ui->comboMotorLayout->setCurrentIndex(mySettings->value("motors/layout", "0").toInt());
}

ROVSettings::~ROVSettings()
{
    delete ui;
}

void ROVSettings::on_pbCancel_clicked()
{
    this->close();
}

void ROVSettings::on_pbSave_clicked()
{
    //Relay Names
    mySettings->setValue("names/relay0", ui->leNameRelay0->text());
    mySettings->setValue("names/relay1", ui->leNameRelay1->text());
    mySettings->setValue("names/relay2", ui->leNameRelay2->text());

    //Units
    mySettings->setValue("units/depth", ui->leUnitsDepth->text());
    mySettings->setValue("units/sensor0", ui->leUnitsSensor0->text());
    mySettings->setValue("units/sensor1", ui->leUnitsSensor1->text());

    //Thresholds
    mySettings->setValue("thresholds/depth", ui->sbDepth->value());
    mySettings->setValue("threshold/voltage", ui->sbVoltage->value());

    //Motors
    mySettings->setValue("motors/layout", ui->comboMotorLayout->currentIndex());

    //Then close the window
    emit callLoadSettings();    //emit a signal for the mainwindow to catch to force it to reload the settings
    this->close();
}
