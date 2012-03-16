#ifndef ROVMAPPINGS_H
#define ROVMAPPINGS_H

#include <QDialog>

namespace Ui {
    class ROVMappings;
}

class ROVMappings : public QDialog
{
    Q_OBJECT

public:
    explicit ROVMappings(QWidget *parent = 0);
    ~ROVMappings();

private:
    Ui::ROVMappings *ui;
};

#endif // ROVMAPPINGS_H
