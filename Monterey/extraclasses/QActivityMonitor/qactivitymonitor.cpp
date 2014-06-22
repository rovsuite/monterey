#include "qactivitymonitor.h"

QActivityMonitor::QActivityMonitor(QTextEdit* textEdit, QObject *parent) :
    QObject(parent)
{
    activityDisplay = textEdit;
    autoscroll = true;
    setColors(Qt::white, Qt::green, Qt::yellow, Qt::red);
}

void QActivityMonitor::display(QString message, MsgType type)
{
    QString toDisplay("(");
    toDisplay.append(time.currentTime().toString("hh:mm:ss"));
    toDisplay.append(")");
    toDisplay.append(" ");
    toDisplay.append(message);

    //Color the text
    QTextCharFormat tf;
    tf = activityDisplay->currentCharFormat();
    QBrush oldForeground = tf.foreground();
    if(mColorMap.contains(type))
    {
        tf.setForeground(QBrush(mColorMap.value(type, oldForeground.color())));
    }

    activityDisplay->setCurrentCharFormat(tf);

    activityDisplay->append(toDisplay);

    //Restore old colors
    tf.setForeground(oldForeground);
    activityDisplay->setCurrentCharFormat(tf);

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

void QActivityMonitor::setColors(QColor info, QColor good, QColor warn, QColor bad)
{
    mColorMap.clear();
    mColorMap.insert(MsgType::Info, info);
    mColorMap.insert(MsgType::Good, good);
    mColorMap.insert(MsgType::Warn, warn);
    mColorMap.insert(MsgType::Bad, bad);
}
