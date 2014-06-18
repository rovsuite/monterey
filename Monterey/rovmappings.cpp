#include "rovmappings.h"
#include "ui_rovmappings.h"
#include "mainwindow.h"

#include <QLayout>
#include <QtDebug>
#include <QMessageBox>

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

        //Load axes
        ui->sbVX->setValue(p->controller->getAxisX());
        ui->sbVY->setValue(p->controller->getAxisY());
        ui->sbVZ->setValue(p->controller->getAxisZ());
        ui->sbV->setValue(p->controller->getAxisV());
        ui->sbTL->setValue(p->controller->getAxisL());
        ui->sbTR->setValue(p->controller->getAxisR());

        //Load relay settings
        QVBoxLayout *buttonLayout = ui->verticalLayoutButtons;
        QVBoxLayout *hatLayout = ui->verticalLayoutHats;
        for(int i=0; i<p->controller->relayMappings.count(); i++)
        {
            //Load buttons
            QHBoxLayout *newLayout = new QHBoxLayout(this);
            QLabel *le = new QLabel(this);
            QComboBox *cb = new QComboBox(this);
            relayButtons.append(cb);

            for(int b=0; b < p->controller->getJoystickNumberButtons(); b++)    //add each button
            {
                cb->addItem(QString::number(b));
            }
            cb->addItem("Disable"); //add option to disable

            le->setText(p->controller->rov()->relays.at(i).name);
            cb->setCurrentIndex(p->controller->relayMappings[i].button);

            newLayout->addWidget(le);
            newLayout->addWidget(cb);
            buttonLayout->addLayout(newLayout);

            //Load hats
            QHBoxLayout *newHat = new QHBoxLayout(this);
            QLabel *label = new QLabel(this);
            QLineEdit *lineEdit = new QLineEdit(this);
            relayHats.append(lineEdit);

            label->setText(p->controller->rov()->relays.at(i).name);
            label->setAlignment(Qt::AlignLeft);
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            lineEdit->setFixedWidth(100);
            lineEdit->setAlignment(Qt::AlignHCenter);
            lineEdit->setText(QString::number(p->controller->relayMappings[i].hat));
            newHat->addWidget(label);
            newHat->addWidget(lineEdit);
            hatLayout->addLayout(newHat);
        }

        //Load servo settings
        for(int i=0; i < p->controller->servoMappings.count(); i++)
        {
            //Load buttons
            QHBoxLayout *newLayoutUp = new QHBoxLayout(this);
            QLabel *leUp = new QLabel(this);
            QComboBox *cbUp = new QComboBox(this);
            servoButtonsUp.append(cbUp);

            QHBoxLayout *newLayoutDown = new QHBoxLayout(this);
            QLabel *leDown = new QLabel(this);
            QComboBox *cbDown = new QComboBox(this);
            servoButtonsDown.append(cbDown);

            for(int b=0; b < p->controller->getJoystickNumberButtons(); b++)
            {
                cbUp->addItem(QString::number(b));
                cbDown->addItem(QString::number(b));
            }
            cbUp->addItem("Disable");
            cbDown->addItem("Disable");

            leUp->setText(p->controller->rov()->servos.at(i).name + " (Up)");
            cbUp->setCurrentIndex(p->controller->servoMappings[i].buttonUp);

            leDown->setText(p->controller->rov()->servos.at(i).name + " (Down)");
            cbDown->setCurrentIndex(p->controller->servoMappings[i].buttonDown);

            newLayoutUp->addWidget(leUp);
            newLayoutUp->addWidget(cbUp);
            buttonLayout->addLayout(newLayoutUp);

            newLayoutDown->addWidget(leDown);
            newLayoutDown->addWidget(cbDown);
            buttonLayout->addLayout(newLayoutDown);

            //Load hats
            QHBoxLayout *newHatUp = new QHBoxLayout(this);
            QLabel *labelUp = new QLabel(this);
            QLineEdit *lineEditUp = new QLineEdit(this);
            servoHatsUp.append(lineEditUp);

            QHBoxLayout *newHatDown = new QHBoxLayout(this);
            QLabel *labelDown = new QLabel(this);
            QLineEdit *lineEditDown = new QLineEdit(this);
            servoHatsDown.append(lineEditDown);

            labelUp->setText(p->controller->rov()->servos.at(i).name + " (Up)");
            labelUp->setAlignment(Qt::AlignLeft);
            labelUp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            lineEditUp->setFixedWidth(100);
            lineEditUp->setAlignment(Qt::AlignHCenter);
            lineEditUp->setText(QString::number(p->controller->servoMappings[i].hatUp));
            newHatUp->addWidget(labelUp);
            newHatUp->addWidget(lineEditUp);
            hatLayout->addLayout(newHatUp);

            labelDown->setText(p->controller->rov()->servos.at(i).name + " (Down)");
            labelDown->setAlignment(Qt::AlignLeft);
            labelDown->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            lineEditDown->setFixedWidth(100);
            lineEditDown->setAlignment(Qt::AlignHCenter);
            lineEditDown->setText(QString::number(p->controller->servoMappings[i].hatDown));
            newHatDown->addWidget(labelDown);
            newHatDown->addWidget(lineEditDown);
            hatLayout->addLayout(newHatDown);
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Mappings dialog is not fully functional.");
        msgBox.setInformativeText("Please attach a joystick for full functionality.");
        msgBox.exec();
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
    if(p->controller->isJoyAttached())
    {
        //Set the axes
        p->controller->setAxisL(ui->sbTL->value());
        p->controller->setAxisR(ui->sbTR->value());
        p->controller->setAxisV(ui->sbV->value());
        p->controller->setAxisX(ui->sbVX->value());
        p->controller->setAxisY(ui->sbVY->value());
        p->controller->setAxisZ(ui->sbVZ->value());

        //Set the relays
        for(int i=0; i<p->controller->relayMappings.count(); i++)
        {
            p->controller->relayMappings[i].button = relayButtons.at(i)->currentIndex();
            p->controller->relayMappings[i].hat = relayHats.at(i)->text().toInt();
        }

        //Set the servos
        for(int i=0; i < p->controller->servoMappings.count(); i++)
        {
            p->controller->servoMappings[i].buttonUp = servoButtonsUp.at(i)->currentIndex();
            p->controller->servoMappings[i].buttonDown = servoButtonsDown.at(i)->currentIndex();

            p->controller->servoMappings[i].hatUp = servoHatsUp.at(i)->text().toInt();
            p->controller->servoMappings[i].hatDown = servoHatsDown.at(i)->text().toInt();
        }

        p->controller->saveSettings();  //save the settings
    }

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
