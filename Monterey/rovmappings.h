#ifndef ROVMAPPINGS_H
#define ROVMAPPINGS_H

#include <QDialog>
#include <QTimer>

namespace Ui {
    class ROVMappings;
}

class ROVMappings : public QDialog
{
    Q_OBJECT

public:
    explicit ROVMappings(QWidget *parent = 0);
    ~ROVMappings();

private slots:
    void on_pbSave_clicked();

    void on_pbCancel_clicked();

    void updateDisplay();

private:
    Ui::ROVMappings *ui;
    QTimer *updateTimer;
};

#endif // ROVMAPPINGS_H
