#include "rovsettings.h"
#include "ui_rovsettings.h"
#include "mainwindow.h"

ROVSettings::ROVSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVSettings)
{
    ui->setupUi(this);

    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    ui->leNameRelay0->setText(p->controller->rov->listRelays[0]->getName());
    ui->leNameRelay1->setText(p->controller->rov->listRelays[1]->getName());
    ui->leNameRelay2->setText(p->controller->rov->listRelays[2]->getName());

    ui->leUnitsDepth->setText(p->controller->rov->sensorDepth->getUnits());
    ui->leUnitsSensor0->setText(p->controller->rov->sensorOther0->getUnits());
    ui->leUnitsSensor1->setText(p->controller->rov->sensorOther1->getUnits());

    ui->sbDepth->setValue(p->controller->rov->sensorDepth->getThreshold());
    ui->sbVoltage->setValue(p->controller->rov->sensorVoltage->getThreshold());

    ui->comboMotorLayout->setCurrentIndex((int)p->controller->getMotorLayout());
    ui->cbBilinearEnabled->setChecked(p->controller->getBilinearEnabled());
    ui->sbBilinearRatio->setValue(p->controller->getBilinearRatio());
    ui->sbBilinearThreshold->setValue(p->controller->getBilinearThreshold());
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
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());

    //Adjust the settings
    p->controller->rov->listRelays[0]->setName(ui->leNameRelay0->text());
    p->controller->rov->listRelays[1]->setName(ui->leNameRelay1->text());
    p->controller->rov->listRelays[2]->setName(ui->leNameRelay2->text());
    p->controller->rov->sensorDepth->setUnits(ui->leUnitsDepth->text());
    p->controller->rov->sensorOther0->setUnits(ui->leUnitsSensor0->text());
    p->controller->rov->sensorOther1->setUnits(ui->leUnitsSensor1->text());
    p->controller->rov->sensorDepth->setThreshold(ui->sbDepth->value());
    p->controller->rov->sensorVoltage->setThreshold(ui->sbVoltage->value());
    p->controller->setMotorLayout(ui->comboMotorLayout->currentIndex());
    p->controller->setBilinearRatio(ui->sbBilinearRatio->value());
    p->controller->setBilinearThreshold(ui->sbBilinearThreshold->value());
    p->controller->setBilinearEnabled(ui->cbBilinearEnabled->isChecked());

    //Save the settings
    p->controller->saveSettings();

    //Then close the window
    emit callLoadSettings();    //emit a signal for the mainwindow to catch to force it to reload the settings
    this->close();
}
