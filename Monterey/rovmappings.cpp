#include "rovmappings.h"
#include "ui_rovmappings.h"
#include "mainwindow.h"

ROVMappings::ROVMappings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVMappings)
{
    ui->setupUi(this);

    //Load the existing mappings
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    if(p->controller->isJoyAttached())
    {
        QList<QSpinBox*> spinBoxes = this->findChildren<QSpinBox*>();
        foreach(QSpinBox* sb, spinBoxes)
        {
            sb->setMaximum(p->controller->getJoystickNumberAxes() - 1);
        }
        ui->sbVX->setValue(p->controller->getAxisX());
        ui->sbVY->setValue(p->controller->getAxisY());
        ui->sbVZ->setValue(p->controller->getAxisZ());
        ui->sbV->setValue(p->controller->getAxisV());
        ui->sbTL->setValue(p->controller->getAxisL());
        ui->sbTR->setValue(p->controller->getAxisR());
    }

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
    updateTimer->start(50);
}

ROVMappings::~ROVMappings()
{
    delete ui;
}

void ROVMappings::on_pbSave_clicked()
{
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    p->controller->setAxisL(ui->sbTL->value());
    p->controller->setAxisR(ui->sbTR->value());
    p->controller->setAxisV(ui->sbV->value());
    p->controller->setAxisX(ui->sbVX->value());
    p->controller->setAxisY(ui->sbVY->value());
    p->controller->setAxisZ(ui->sbVZ->value());

    p->controller->saveSettings();  //save the settings

    this->close();
}

void ROVMappings::on_pbCancel_clicked()
{
    this->close();
}

void ROVMappings::updateDisplay()
{
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    if(p->controller->isJoyAttached())
    {
        ui->pbTL->setValue(p->controller->joystickAxesValues[ui->sbTL->value()]);
        ui->pbTR->setValue(p->controller->joystickAxesValues[ui->sbTR->value()]);
        ui->pbV->setValue(p->controller->joystickAxesValues[ui->sbV->value()]);
        ui->pbVX->setValue(p->controller->joystickAxesValues[ui->sbVX->value()]);
        ui->pbVY->setValue(p->controller->joystickAxesValues[ui->sbVY->value()]);
        ui->pbVZ->setValue(p->controller->joystickAxesValues[ui->sbVZ->value()]);
    }
}
