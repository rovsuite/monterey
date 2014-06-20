#ifndef ROVSETTINGS_H
#define ROVSETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QLineEdit>
#include "extraclasses/QROV/qrov.h"

namespace Ui {
    class ROVSettings;
}

class ROVSettings : public QDialog
{
    Q_OBJECT

public:
    explicit ROVSettings(QWidget *parent = 0);
    ~ROVSettings();

signals:
    void callLoadSettings();

private slots:
    void on_pbCancel_clicked();
    void on_pbSave_clicked();
    void onAutoGenerateVideoFeedUrlClicked(bool enabled);

private:
    Ui::ROVSettings *ui;
    QSettings *mySettings;
    IpVideoFeed videoFeed;
    QList<QLineEdit*> relayNames;
};

#endif // ROVSETTINGS_H
