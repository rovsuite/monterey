#ifndef QACTIVITYMONITOR_H
#define QACTIVITYMONITOR_H

//Written by Chris Konstad
//March 2012

#include <QObject>
#include <QTextEdit>
#include <QTime>
#include <QtDebug>

class QActivityMonitor : public QObject
{
    Q_OBJECT
public:
    explicit QActivityMonitor(QObject *parent = 0);

signals:

public slots:
    void display(QString message);
    void clear();
    void setTextEdit(QTextEdit* textEdit);
    void setAutoscroll(bool enabled);

private:
    bool autoscroll;
    QTextEdit* activityDisplay;
    QTime time;

};

#endif // QACTIVITYMONITOR_H
