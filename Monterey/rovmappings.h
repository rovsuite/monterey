#ifndef ROVMAPPINGS_H
#define ROVMAPPINGS_H

#include <QDialog>
#include <QTimer>
#include <QLineEdit>
#include <QComboBox>
#include <QList>

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
    int numAxes;

    //Keep track of relay settings
    QList<QComboBox*> relayButtons;
    QList<QLineEdit*> relayHats;

    //Keep track of servo settings
    QList<QComboBox*> servoButtonsUp;
    QList<QLineEdit*> servoHatsUp;
    QList<QComboBox*> servoButtonsDown;
    QList<QLineEdit*> servoHatsDown;
};

#endif // ROVMAPPINGS_H
