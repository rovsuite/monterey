#ifndef ROVSETTINGS_H
#define ROVSETTINGS_H

#include <QDialog>

namespace Ui {
    class ROVSettings;
}

class ROVSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ROVSettings(QWidget *parent = 0);
    ~ROVSettings();

private:
    Ui::ROVSettings *ui;
};

#endif // ROVSETTINGS_H
