#include "rovsettings.h"
#include "ui_rovsettings.h"
#include "mainwindow.h"
#include "extraclasses/IpVideoFeedSettingsWidget/ipvideofeedsettingswidget.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>

ROVSettings::ROVSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVSettings)
{
    ui->setupUi(this);
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());

    ui->cbBilinearEnabled->setChecked(p->controller->getBilinearEnabled());
    ui->sbBilinearRatio->setValue(p->controller->getBilinearRatio());

    QStringList joystickNames = p->controller->getJoystickNames();
    foreach(QString name, joystickNames)
    {
        ui->listJoysticks->addItem(name);
    }
    ui->listJoysticks->item(0)->setSelected(true);

   videoFeeds = p->controller->rov()->videoFeeds;
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
    p->controller->setBilinearRatio(ui->sbBilinearRatio->value());
    p->controller->setBilinearEnabled(ui->cbBilinearEnabled->isChecked());

    videoFeeds[0] = ui->ipVideoFeedSettings0->getIpVideoFeed();
    p->controller->rov()->videoFeeds = videoFeeds;

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
        if(!p->controller->rov()->piData->ipAddress().isNull())
            url.append(p->controller->rov()->piData->ipAddress().toString());
        else
            url.append("127.0.0.1");
        url.append(":8080/javascript_simple.html");
        ui->ipVideoFeedSettings0->setUrl(url);
        ui->ipVideoFeedSettings0->disableUrlEditing();
    }
    else
    {
        ui->ipVideoFeedSettings0->enableUrlEditing();
        ui->ipVideoFeedSettings0->setUrl(p->controller->rov()->videoFeeds.at(0)->url().toString());
    }
}
