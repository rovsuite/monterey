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

#include "rovsettings.h"
#include "ui_rovsettings.h"
#include "mainwindow.h"
#include "extraclasses/IpVideoFeedSettingsWidget/ipvideofeedsettingswidget.h"

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

    ui->leNamesSensor0->setText(p->controller->rov->sensorOther0->getName());
    ui->leNamesSensor1->setText(p->controller->rov->sensorOther1->getName());

    ui->sbDepth->setValue(p->controller->rov->sensorDepth->getThreshold());
    ui->sbVoltage->setValue(p->controller->rov->sensorVoltage->getThreshold());

    ui->comboMotorLayout->setCurrentIndex((int)p->controller->getMotorLayout());
    ui->cbBilinearEnabled->setChecked(p->controller->getBilinearEnabled());
    ui->sbBilinearRatio->setValue(p->controller->getBilinearRatio());

    QStringList joystickNames = p->controller->getJoystickNames();
    foreach(QString name, joystickNames)
    {
        ui->listJoysticks->addItem(name);
    }
    ui->listJoysticks->item(0)->setSelected(true);

   videoFeeds = p->controller->rov->getVideoFeeds();
   ui->ipVideoFeedSettings0->setIpVideoFeed(videoFeeds.at(0));
   connect(ui->ipVideoFeedSettings0, SIGNAL(autoGenerateClicked(bool)), this, SLOT(onAutoGenerateVideoFeedUrlClicked(bool)));

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
    p->controller->rov->sensorOther0->setName(ui->leNamesSensor0->text());
    p->controller->rov->sensorOther1->setName(ui->leNamesSensor1->text());
    p->controller->rov->sensorDepth->setThreshold(ui->sbDepth->value());
    p->controller->rov->sensorVoltage->setThreshold(ui->sbVoltage->value());
    p->controller->setMotorLayout(ui->comboMotorLayout->currentIndex());
    p->controller->setBilinearRatio(ui->sbBilinearRatio->value());
    p->controller->setBilinearEnabled(ui->cbBilinearEnabled->isChecked());

    videoFeeds[0] = ui->ipVideoFeedSettings0->getIpVideoFeed();
    p->controller->rov->setVideoFeeds(videoFeeds);

    //Save the settings
    p->controller->saveSettings();

    //Then close the window
    emit callLoadSettings();    //emit a signal for the mainwindow to catch to force it to reload the settings
    this->close();
}

void ROVSettings::onAutoGenerateVideoFeedUrlClicked(bool enabled)
{
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    if(enabled)
    {
        QString url;
        url.append("http://");
        if(p->controller->rov->piData->ipAddress())
            url.append(p->controller->rov->piData->ipAddress()->toString());
        else
            url.append("127.0.0.1");
        url.append(":8080/javascript_simple.html");
        ui->ipVideoFeedSettings0->setUrl(url);
        ui->ipVideoFeedSettings0->disableUrlEditing();
    }
    else
    {
        ui->ipVideoFeedSettings0->enableUrlEditing();
        ui->ipVideoFeedSettings0->setUrl(p->controller->rov->getVideoFeeds().at(0)->url().toString());
    }
}
