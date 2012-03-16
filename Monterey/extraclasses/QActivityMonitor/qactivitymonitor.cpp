#include "qactivitymonitor.h"

QActivityMonitor::QActivityMonitor(QObject *parent) :
    QObject(parent)
{
    autoscroll = true;
}

void QActivityMonitor::display(QString message)
{
    QString toDisplay("(");
    toDisplay.append(time.currentTime().toString("hh:mm:ss"));
    toDisplay.append(")");
    toDisplay.append('\t');
    toDisplay.append(message);

    activityDisplay->append(toDisplay);

    if(autoscroll)
    {
        QTextCursor c = activityDisplay->textCursor();
        c.movePosition(QTextCursor::End);
        activityDisplay->setTextCursor(c);
    }
}

void QActivityMonitor::clear()
{
    activityDisplay->clear();
}

void QActivityMonitor::setTextEdit(QTextEdit* textEdit)
{
    activityDisplay = textEdit;
    activityDisplay->setReadOnly(true);
}

void QActivityMonitor::setAutoscroll(bool enabled)
{
    autoscroll = enabled;
}
