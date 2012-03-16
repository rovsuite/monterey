#include "rovmappings.h"
#include "ui_rovmappings.h"

ROVMappings::ROVMappings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ROVMappings)
{
    ui->setupUi(this);
}

ROVMappings::~ROVMappings()
{
    delete ui;
}
