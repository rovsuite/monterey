#include "udpcapture.h"

UdpCapture::UdpCapture(unsigned int port, int timeoutMs, QObject *parent) :
    QObject(parent)
{
    m_packet = "";

    m_socket = new QUdpSocket(this);
    m_socket->bind(port);

    m_host = new QHostAddress;

    mComStatus = false;

    mComTimer = new QTimer(this);
    mComTimer->setInterval(timeoutMs);
    mComTimer->start();

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(processPacket()));
    connect(mComTimer, SIGNAL(timeout()), this, SLOT(comTimeout()));
}

UdpCapture::~UdpCapture()
{
    m_socket->close();  //m_socket is automatically deleted because of parent/child relationship
}


QString UdpCapture::packetAsString()
{
    return m_packet;
}

void UdpCapture::setPort(unsigned int port)
{
    m_socket->close();
    m_socket->bind(port);
}

void UdpCapture::closePort()
{
    if(m_socket->isOpen())
        m_socket->close();
}

void UdpCapture::processPacket()
{
    QByteArray datagram;
    do
    {
        datagram.resize(m_socket->pendingDatagramSize());
        m_socket->readDatagram(datagram.data(), datagram.size(), m_host);
    }
    while(m_socket->hasPendingDatagrams());
    m_packet = (tr("%1").arg(datagram.data()));

    comOk();

    emit packetProcessed(m_packet);
}

void UdpCapture::comTimeout()
{
    if(mComStatus == true)
    {
        emit comLost();
        mComStatus = false;
        emit comChanged(mComStatus);
    }
}

void UdpCapture::comOk()
{
    mComTimer->start();

    if(mComStatus == false)
    {
        emit comGained();
        mComStatus = true;
        emit comChanged(mComStatus);
    }
}
