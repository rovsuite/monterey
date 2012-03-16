#ifndef ROVABOUT_H
#define ROVABOUT_H

#include <QDialog>
#include <QFile>
#include <QTextStream>

namespace Ui {
    class ROVAbout;
}

class ROVAbout : public QDialog
{
    Q_OBJECT

public:
    explicit ROVAbout(QWidget *parent = 0);
    ~ROVAbout();

private:
    Ui::ROVAbout *ui;
};

#endif // ROVABOUT_H
