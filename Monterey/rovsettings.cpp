#include "rovsettings.h"
#include "ui_rovsettings.h"

ROVSettings::ROVSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVSettings)
{
    ui->setupUi(this);
}

ROVSettings::~ROVSettings()
{
    delete ui;
}
