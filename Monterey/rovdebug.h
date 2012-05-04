#ifndef ROVDEBUG_H
#define ROVDEBUG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
    class ROVDebug;
}

class ROVDebug : public QDialog
{
    Q_OBJECT

public:
    explicit ROVDebug(QWidget *parent = 0);
    ~ROVDebug();

private slots:
    void updateGUI();
    void displayTIBOPacket(QString toDisp);
    void displayTOBIPacket(QString toDisp);

private:
    Ui::ROVDebug *ui;
    QTimer *updateTimer;
};

#endif // ROVDEBUG_H
