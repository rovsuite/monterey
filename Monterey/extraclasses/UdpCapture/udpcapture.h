#ifndef UDPCAPTURE_H
#define UDPCAPTURE_H
/*
 * Copyright (C) <2013> Chris Konstad <chriskon149@gmail.com>
 *
 * NOTE: DON'T FORGET TO ADD "QT += network" TO YOUR *.PRO FILE!
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QObject>
#include <QtNetwork>
#include <QString>
#include <QByteArray>
#include <QHostAddress>

class UdpCapture : public QObject
{
    Q_OBJECT
public:
    explicit UdpCapture(unsigned int port, int timeoutMs,QObject *parent = 0);
    ~UdpCapture();
    
signals:
    void packetProcessed(QString packetString);
    void comLost();
    void comGained();
    void comChanged(bool status);
    
public slots:
    QString packetAsString();
    void setPort(unsigned int port);
    void closePort();
    QHostAddress senderHostAddress() { return *m_host; }
    QUdpSocket* socket() { return m_socket; }
    bool comStatus() { return mComStatus; }

private slots:
    void processPacket();
    void comTimeout();
    void comOk();

private:
    QUdpSocket *m_socket;
    QString m_packet;
    QHostAddress *m_host;
    QTimer *mComTimer;
    bool mComStatus;
    
};

#endif // UDPCAPTURE_H
