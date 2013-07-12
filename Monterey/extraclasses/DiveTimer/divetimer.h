#ifndef DIVETIMER_H
#define DIVETIMER_H

#include <QObject>
#include <QTimer>
#include <QString>

class DiveTimer : public QObject
{
    Q_OBJECT
public:
    explicit DiveTimer(QObject *parent = 0);
    
signals:
    
public slots:
    void start();
    void pause();
    void resume();
    void reset();
    QString diveTimeString();
    bool hasStarted();

private slots:
    void onTick();  //called every second
    
private:
    QTimer *timer;
    unsigned int secondsElapsed;
    bool paused;
    bool started;
};

#endif // DIVETIMER_H
