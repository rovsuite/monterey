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
        //Load maximum axes
        QList<QSpinBox*> spinBoxes = this->findChildren<QSpinBox*>();
        foreach(QSpinBox* sb, spinBoxes)
        {
            sb->setMaximum(p->controller->getJoystickNumberAxes() - 1);
        }

        //Load maximum buttons
        QList<QComboBox*> comboButtons;
        comboButtons.append(ui->cbR0But);
        comboButtons.append(ui->cbR1But);
        comboButtons.append(ui->cbR2But);
        comboButtons.append(ui->cbS0DBut);
        comboButtons.append(ui->cbS0IBut);
        comboButtons.append(ui->cbS1DBut);
        comboButtons.append(ui->cbS1IBut);
        foreach(QComboBox* cb, comboButtons)    //for each combo box
        {
            for(int b=0; b < p->controller->getJoystickNumberButtons(); b++)    //add each button
            {
                cb->addItem(QString::number(b));
            }
            cb->addItem("Disable"); //add option to disable
        }
        //Load axes
        ui->sbVX->setValue(p->controller->getAxisX());
        ui->sbVY->setValue(p->controller->getAxisY());
        ui->sbVZ->setValue(p->controller->getAxisZ());
        ui->sbV->setValue(p->controller->getAxisV());
        ui->sbTL->setValue(p->controller->getAxisL());
        ui->sbTR->setValue(p->controller->getAxisR());

        //Load buttons
        ui->cbR0But->setCurrentIndex(p->controller->rov->listRelays[0]->getButton());
        ui->cbR1But->setCurrentIndex(p->controller->rov->listRelays[1]->getButton());
        ui->cbR2But->setCurrentIndex(p->controller->rov->listRelays[2]->getButton());
        ui->cbS0DBut->setCurrentIndex(p->controller->rov->listServos[0]->getButtonDown());
        ui->cbS0IBut->setCurrentIndex(p->controller->rov->listServos[0]->getButtonUp());
        ui->cbS1DBut->setCurrentIndex(p->controller->rov->listServos[1]->getButtonDown());
        ui->cbS1IBut->setCurrentIndex(p->controller->rov->listServos[1]->getButtonUp());

        //Load hats
        ui->leR0Hat->setText(QString::number(p->controller->rov->listRelays[0]->getHat()));
        ui->leR1Hat->setText(QString::number(p->controller->rov->listRelays[1]->getHat()));
        ui->leR2Hat->setText(QString::number(p->controller->rov->listRelays[2]->getHat()));
        ui->leS0DHat->setText(QString::number(p->controller->rov->listServos[0]->getHatDown()));
        ui->leS0IHat->setText(QString::number(p->controller->rov->listServos[0]->getHatUp()));
        ui->leS1DHat->setText(QString::number(p->controller->rov->listServos[1]->getHatDown()));
        ui->leS1IHat->setText(QString::number(p->controller->rov->listServos[1]->getHatUp()));
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
    //Set the axes
    p->controller->setAxisL(ui->sbTL->value());
    p->controller->setAxisR(ui->sbTR->value());
    p->controller->setAxisV(ui->sbV->value());
    p->controller->setAxisX(ui->sbVX->value());
    p->controller->setAxisY(ui->sbVY->value());
    p->controller->setAxisZ(ui->sbVZ->value());

    //Set the buttons
    p->controller->rov->listRelays[0]->setButton(ui->cbR0But->currentIndex());
    p->controller->rov->listRelays[1]->setButton(ui->cbR1But->currentIndex());
    p->controller->rov->listRelays[2]->setButton(ui->cbR2But->currentIndex());
    p->controller->rov->listServos[0]->setButtonDown(ui->cbS0DBut->currentIndex());
    p->controller->rov->listServos[0]->setButtonUp(ui->cbS0IBut->currentIndex());
    p->controller->rov->listServos[1]->setButtonDown(ui->cbS1DBut->currentIndex());
    p->controller->rov->listServos[1]->setButtonUp(ui->cbS1IBut->currentIndex());

    //Set the hats
    p->controller->rov->listRelays[0]->setHat(ui->leR0Hat->text().toInt());
    p->controller->rov->listRelays[1]->setHat(ui->leR1Hat->text().toInt());
    p->controller->rov->listRelays[2]->setHat(ui->leR2Hat->text().toInt());
    p->controller->rov->listServos[0]->setHatDown(ui->leS0DHat->text().toInt());
    p->controller->rov->listServos[0]->setHatUp(ui->leS0IHat->text().toInt());
    p->controller->rov->listServos[1]->setHatDown(ui->leS1DHat->text().toInt());
    p->controller->rov->listServos[1]->setHatUp(ui->leS1IHat->text().toInt());

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
        //Display axes values
        ui->pbTL->setValue(p->controller->joystickAxesValues[ui->sbTL->value()]);
        ui->pbTR->setValue(p->controller->joystickAxesValues[ui->sbTR->value()]);
        ui->pbV->setValue(p->controller->joystickAxesValues[ui->sbV->value()]);
        ui->pbVX->setValue(p->controller->joystickAxesValues[ui->sbVX->value()]);
        ui->pbVY->setValue(p->controller->joystickAxesValues[ui->sbVY->value()]);
        ui->pbVZ->setValue(p->controller->joystickAxesValues[ui->sbVZ->value()]);

        //Display hat value
        ui->leCurrentHat->setText(QString::number(p->controller->getJoystickCurrentHatValue()));

        //Display currently pressed buttons
        QString temp;
        QList<int> tempList = p->controller->getJoystickCurrentButtonValue();
        if(tempList[0] == -1)   //if no button pressed
        {
            temp.append(" ");
        }
        else if(tempList.count() == 1)   //if only one item
        {
            temp.append(QString::number(tempList[0]));
        }
        else    //if multiple buttons are pressed
        {
            temp.append(QString::number(tempList[0]));
            for(int i=1;i<tempList.count();i++)
            {
                temp.append(", ");
                temp.append(QString::number(i));
            }
        }
        ui->leCurrentButton->setText(temp);
    }
}
