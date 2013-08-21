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

#include "rovabout.h"
#include "ui_rovabout.h"
#include <QDebug>

ROVAbout::ROVAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVAbout)
{
    ui->setupUi(this);

    //Display the text
    QFile aboutFile(":/text/resources/plainabout.txt");    //held in the qresource

    if(!aboutFile.open(QIODevice::ReadOnly))   //if file is readable
    {
        qDebug() << "Couldn't open text file";
    }

    QTextStream in(&aboutFile);
    QString aboutText = in.readAll();

    ui->teAbout->append(aboutText);
    ui->teAbout->moveCursor(QTextCursor::Start);

    aboutFile.close();  //close the file

    //Display the photo
    ui->labelIcon->setPixmap(QPixmap(":/pictures/resources/montereySmall.png", 0, Qt::AutoColor));
}

ROVAbout::~ROVAbout()
{
    delete ui;
}
