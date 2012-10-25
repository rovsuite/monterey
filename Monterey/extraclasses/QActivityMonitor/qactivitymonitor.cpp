/*
    Copyright (C) 2012  Chris Konstad (chriskon149@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qactivitymonitor.h"

QActivityMonitor::QActivityMonitor(QTextEdit* textEdit, QObject *parent) :
    QObject(parent)
{
    activityDisplay = textEdit;
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
