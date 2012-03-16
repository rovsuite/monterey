//This class can be used to check when a bool value is changed from one state to another
//Written by Chris Konstad (02/24/2012)

//There is no warranty provided with this simple library
//Feel free to distribute, modify, use, etc this library, just make sure
//to give credit to the original author

#ifndef QBOOLMONITOR_H
#define QBOOLMONITOR_H

#include <QObject>
#include <QString>

class QBoolMonitor : public QObject
{
    Q_OBJECT
public:
    explicit QBoolMonitor(QObject *parent = 0);

signals:
    void stateChanged();
    void stateChangedTo(bool state);
    void stateChangedMessage(QString message);

public slots:
    void compareState(bool input);
    void setComparisonState(bool overrideState);
    bool getComparisonState();
    void setMessage(QString m);
    QString getMessage();

private:
    bool lastState;
    bool currentState;
    QString message;

private slots:

};

#endif // QBOOLMONITOR_H
