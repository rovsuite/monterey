#include "pidata.h"

PiData::PiData(QObject *parent) :
    QObject(parent)
{
    m_tempC = 0;
    m_uptimeS = 0;
    m_usedMemory = 0;
    m_usedCpu = 0;
    m_ipAddress = QHostAddress::Null;
}

double PiData::tempC() const
{
    return m_tempC;
}

int PiData::uptimeS() const
{
    return m_uptimeS;
}

QHostAddress PiData::ipAddress() const
{
    return m_ipAddress;
}

double PiData::usedMemory() const
{
    return m_usedMemory;
}

double PiData::usedCpu() const
{
    return m_usedCpu;
}

void PiData::setTempC(double arg)
{
    if (m_tempC != arg) {
        m_tempC = arg;
        emit tempCChanged(arg);
    }
}

void PiData::setUptimeS(int arg)
{
    m_uptimeS = arg;
}

void PiData::setIpAddress(QHostAddress arg)
{
    m_ipAddress = arg;
}

void PiData::setUsedMemory(double arg)
{
    if (m_usedMemory != arg) {
        m_usedMemory = arg;
        emit usedMemoryChanged(arg);
    }
}

void PiData::setUsedCpu(double arg)
{
    if (m_usedCpu != arg) {
        m_usedCpu = arg;
        emit usedCpuChanged(arg);
    }
}
