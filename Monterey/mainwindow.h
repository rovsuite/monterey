#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QActivityMonitor/qactivitymonitor.h"
#include "rovabout.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showAbout();

private:
    Ui::MainWindow *ui;
    QActivityMonitor *activityMonitor;
    ROVAbout *about;
};

#endif // MAINWINDOW_H
