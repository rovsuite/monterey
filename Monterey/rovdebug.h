#ifndef ROVDEBUG_H
#define ROVDEBUG_H

#include <QDialog>

namespace Ui {
    class ROVDebug;
}

class ROVDebug : public QDialog
{
    Q_OBJECT

public:
    explicit ROVDebug(QWidget *parent = 0);
    ~ROVDebug();

private:
    Ui::ROVDebug *ui;
};

#endif // ROVDEBUG_H
