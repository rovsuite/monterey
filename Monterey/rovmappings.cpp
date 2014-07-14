#include "rovmappings.h"
#include "ui_rovmappings.h"
#include "mainwindow.h"

#include <QLayout>
#include <QtDebug>
#include <QMessageBox>
#include <QComboBox>
#include <QTimer>
#include <QLineEdit>

ROVMappings::ROVMappings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVMappings)
{
    ui->setupUi(this);

    //Match any string with (3 digits, 3 digits)
    inputMask = "(900,900)";

    //Load the existing mappings
    MainWindow *p = dynamic_cast<MainWindow *> (this->parentWidget());
    if(p->controller->isJoyAttached())
    {
        const int numAxes = p->controller->getJoystickAxesValues().count();
        const int numButtons = p->controller->getJoystickButtonsPressed().count();
        //Load maximum axes
        QList<QSpinBox*> spinBoxes = this->findChildren<QSpinBox*>();
        foreach(QSpinBox* sb, spinBoxes)
        {
            sb->setMaximum(numAxes-1);
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

            for(int b=0; b < numButtons; b++)    //add each button
            {
                cb->addItem(QString::number(b));
            }
            cb->addItem("Disable"); //add option to disable

            le->setText(p->controller->rov().relays.at(i).name);
            cb->setCurrentIndex(p->controller->relayMappings[i].button);

            newLayout->addWidget(le);
            newLayout->addWidget(cb);
            buttonLayout->addLayout(newLayout);

            //Load hats
            QHBoxLayout *newHat = new QHBoxLayout(this);
            QLabel *label = new QLabel(this);
            QLineEdit *lineEdit = new QLineEdit(this);
            relayHats.append(lineEdit);

            label->setText(p->controller->rov().relays.at(i).name);
            label->setAlignment(Qt::AlignLeft);
            label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            lineEdit->setFixedWidth(100);
            lineEdit->setAlignment(Qt::AlignHCenter);
            lineEdit->setInputMask(inputMask);
            QString current;
            current = "(";
            current += QString::number(p->controller->relayMappings[i].hat);
            current += ",";
            current += QString::number(p->controller->relayMappings[i].dir);
            current += ")";
            lineEdit->setText(getHatString(p->controller->relayMappings[i].hat,
                                           p->controller->relayMappings[i].dir
                                           ));
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

            for(int b=0; b < numButtons; b++)
            {
                cbUp->addItem(QString::number(b));
                cbDown->addItem(QString::number(b));
            }
            cbUp->addItem("Disable");
            cbDown->addItem("Disable");

            leUp->setText(p->controller->rov().servos.at(i).name + " (Up)");
            cbUp->setCurrentIndex(p->controller->servoMappings[i].buttonUp);

            leDown->setText(p->controller->rov().servos.at(i).name + " (Down)");
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

            labelUp->setText(p->controller->rov().servos.at(i).name + " (Up)");
            labelUp->setAlignment(Qt::AlignLeft);
            labelUp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            lineEditUp->setFixedWidth(100);
            lineEditUp->setAlignment(Qt::AlignHCenter);
            lineEditUp->setInputMask(inputMask);
            lineEditUp->setText(getHatString(p->controller->
                                             servoMappings[i].hatUp,
                                             p->controller->
                                             servoMappings[i].dirUp));
            newHatUp->addWidget(labelUp);
            newHatUp->addWidget(lineEditUp);
            hatLayout->addLayout(newHatUp);

            labelDown->setText(p->controller->rov().servos.at(i).name + " (Down)");
            labelDown->setAlignment(Qt::AlignLeft);
            labelDown->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            lineEditDown->setFixedWidth(100);
            lineEditDown->setAlignment(Qt::AlignHCenter);
            lineEditDown->setText(getHatString(p->controller->
                                               servoMappings[i].hatDown,
                                               p->controller->
                                               servoMappings[i].dirDown));
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
            int hat, dir;
            getHat(relayHats.at(i)->text(), hat, dir);
            p->controller->relayMappings[i].hat = hat;
            p->controller->relayMappings[i].dir = dir;
        }

        //Set the servos
        for(int i=0; i < p->controller->servoMappings.count(); i++)
        {
            p->controller->servoMappings[i].buttonUp = servoButtonsUp.at(i)->currentIndex();
            p->controller->servoMappings[i].buttonDown = servoButtonsDown.at(i)->currentIndex();

            int hat, dir;
            getHat(servoHatsUp.at(i)->text(), hat, dir);
            p->controller->servoMappings[i].hatUp = hat;
            p->controller->servoMappings[i].dirUp = dir;

            getHat(servoHatsDown.at(i)->text(), hat, dir);
            p->controller->servoMappings[i].hatDown = hat;
            p->controller->servoMappings[i].dirDown = dir;
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
        QList<int> axesValues = p->controller->getJoystickAxesValues();
        const int numAxes = axesValues.count();
        //Display axes values
        Q_ASSERT(ui->sbTL->value() < numAxes && ui->sbTL->value() >= 0);
        ui->pbTL->setValue(axesValues[ui->sbTL->value()]);
        Q_ASSERT(ui->sbTR->value() < numAxes && ui->sbTR->value() >= 0);
        ui->pbTR->setValue(axesValues[ui->sbTR->value()]);
        Q_ASSERT(ui->sbV->value() < numAxes && ui->sbV->value() >= 0);
        ui->pbV->setValue(axesValues[ui->sbV->value()]);
        Q_ASSERT(ui->sbVX->value() < numAxes && ui->sbVX->value() >= 0);
        ui->pbVX->setValue(axesValues[ui->sbVX->value()]);
        Q_ASSERT(ui->sbVY->value() < numAxes && ui->sbVY->value() >= 0);
        ui->pbVY->setValue(axesValues[ui->sbVY->value()]);
        Q_ASSERT(ui->sbVZ->value() < numAxes && ui->sbVZ->value() >= 0);
        ui->pbVZ->setValue(axesValues[ui->sbVZ->value()]);

        //Display hat value
        QString hatString = "";
        QList<int> hatsPressed = p->controller->getJoystickHatsPressed();
        for(int i=0; i<hatsPressed.count(); i++)
        {
            if(hatsPressed[i] != 0)
            {
                if(!hatString.isEmpty())
                {
                    hatString.append(" ");
                }
                hatString.append(getHatString(i, hatsPressed[i]));
            }
        }
        ui->leCurrentHat->setText(hatString);

        //Display currently pressed buttons
        QString buttonString = "";
        QList<bool> buttonsPressed = p->controller->getJoystickButtonsPressed();
        for(int b=0; b<buttonsPressed.count(); b++)
        {
            if(buttonsPressed[b])
            {
                if(!buttonString.isEmpty())
                {
                    buttonString.append(", ");
                }
                buttonString.append(QString::number(b));
            }
        }
        ui->leCurrentButton->setText(buttonString);
    }
}

QString ROVMappings::getHatString(int hat, int dir)
{
    QString str = "(";
    str += QString::number(hat);
    str += ",";
    str += QString::number(dir);
    str += ")";
    return str;
}

void ROVMappings::getHat(QString str, int& hat, int& dir)
{
    //Remove parentheses
    str.remove(0, 1);
    str.remove(str.length()-1, 1);

    //Split up according to the comma
    QStringList hatValues = str.split(",");
    hat = hatValues[0].toInt();
    dir = hatValues[1].toInt();
}
