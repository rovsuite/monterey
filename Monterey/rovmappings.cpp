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
        numAxes = p->controller->getJoystickNumberAxes();
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
    qDebug() << "Setup the mappings dialog!";
}

ROVMappings::~ROVMappings()
{
    qDebug() << "Mappings dialog closed";
    delete ui;
    this->deleteLater();
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

    delete updateTimer;
    this->close();
}

void ROVMappings::on_pbCancel_clicked()
{
    delete updateTimer;
    this->close();
}

void ROVMappings::updateDisplay()
{
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    if(p->controller->isJoyAttached())
    {
        //Display axes values
        qDebug() << "Displaying 1 axis values";
        Q_ASSERT(ui->sbTL->value() < numAxes && ui->sbTL->value() >= 0);
        ui->pbTL->setValue(p->controller->getJoystickAxesValues(ui->sbTL->value()));
        qDebug() << ui->pbTL->value();
        qDebug() << "Displaying 2 axis values";
        Q_ASSERT(ui->sbTR->value() < numAxes && ui->sbTR->value() >= 0);
        ui->pbTR->setValue(p->controller->getJoystickAxesValues(ui->sbTR->value()));
        qDebug() << ui->pbTR->value();
        qDebug() << "Displaying 3 axis values";
        Q_ASSERT(ui->sbV->value() < numAxes && ui->sbV->value() >= 0);
        ui->pbV->setValue(p->controller->getJoystickAxesValues(ui->sbV->value()));
        qDebug() << ui->pbV->value();
        qDebug() << "Displaying 4 axis values";
        Q_ASSERT(ui->sbVX->value() < numAxes && ui->sbVX->value() >= 0);
        ui->pbVX->setValue(p->controller->getJoystickAxesValues(ui->sbVX->value()));
        qDebug() << ui->pbVX->value();
        qDebug() << "Displaying 5 axis values";
        Q_ASSERT(ui->sbVY->value() < numAxes && ui->sbVY->value() >= 0);
        ui->pbVY->setValue(p->controller->getJoystickAxesValues(ui->sbVY->value()));
        qDebug() << ui->pbVY->value();
        qDebug() << "Displaying 6 axis values";
        Q_ASSERT(ui->sbVZ->value() < numAxes && ui->sbVZ->value() >= 0);
        ui->pbVZ->setValue(p->controller->getJoystickAxesValues(ui->sbVZ->value()));
        qDebug() << ui->pbVZ->value();

        //Display hat value
        qDebug() << "Displaying hat value!";
        ui->leCurrentHat->setText(QString::number(p->controller->getJoystickCurrentHatValue()));

        //Display currently pressed buttons
        qDebug() << "Displaying button values!";
        QString temp;
        QList<int> tempList = p->controller->getJoystickCurrentButtonValue();
        if(tempList.count())
        {
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
                for(int i=0;i<tempList.count();i++) //should fix crash as of 8/1/2012
                {
                    temp.append(QString::number(i));
                    temp.append(", ");
                }
                if(temp.endsWith(QChar(','), Qt::CaseInsensitive))
                    temp.chop(temp.count()-1);  //remove last comma
            }
            ui->leCurrentButton->setText(temp);
        }
        else
            qWarning() << "tempList has no values" << tempList;

    }
}
