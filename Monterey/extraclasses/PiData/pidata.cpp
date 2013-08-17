#include "pidata.h"

PiData::PiData(QObject *parent) :
    QObject(parent)
{
}

double PiData::tempC() const
{
    return m_tempC;
}

int PiData::uptimeMs() const
{
    return m_uptimeMs;
}

QHostAddress *PiData::ipAddress() const
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

void PiData::setUptimeMs(int arg)
{
    m_uptimeMs = arg;
}

void PiData::setIpAddress(QHostAddress *arg)
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
