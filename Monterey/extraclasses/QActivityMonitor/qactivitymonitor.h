#ifndef QACTIVITYMONITOR_H
#define QACTIVITYMONITOR_H

//Written by Chris Konstad
//March 2012

#include <QObject>
#include <QTextEdit>
#include <QTime>
#include <QtDebug>
#include <QMap>
#include <QColor>
#include "../QROVController/qrovcontroller.h"

typedef QROVController::MsgType MsgType;

class QActivityMonitor : public QObject
{
    Q_OBJECT
public:
    explicit QActivityMonitor(QTextEdit* textEdit, QObject *parent = 0);

signals:

public slots:
    void display(QString message, MsgType type);
    void clear();
    void setTextEdit(QTextEdit* textEdit);
    void setAutoscroll(bool enabled);
    void setColors(QColor info, QColor good, QColor warn, QColor bad);

private:
    bool autoscroll;
    QTextEdit* activityDisplay;
    QTime time;
    QMap<MsgType, QColor> mColorMap;

};

#endif // QACTIVITYMONITOR_H
