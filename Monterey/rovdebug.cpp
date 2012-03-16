#include "rovdebug.h"
#include "ui_rovdebug.h"

ROVDebug::ROVDebug(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVDebug)
{
    ui->setupUi(this);
}

ROVDebug::~ROVDebug()
{
    delete ui;
}
