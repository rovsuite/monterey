#include "rovabout.h"
#include "ui_rovabout.h"
#include <QDebug>

ROVAbout::ROVAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVAbout)
{
    ui->setupUi(this);
    QFile aboutFile(":/text/resources/plainabout.txt");    //held in the qresource

    if(!aboutFile.open(QIODevice::ReadOnly))   //if file is readable
    {
        qDebug() << "Couldn't open file";
    }

    QTextStream in(&aboutFile);
    QString aboutText = in.readAll();

    ui->teAbout->append(aboutText);

    aboutFile.close();  //close the file
}

ROVAbout::~ROVAbout()
{
    delete ui;
}
