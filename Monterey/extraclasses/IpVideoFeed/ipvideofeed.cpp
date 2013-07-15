#include "ipvideofeed.h"

IpVideoFeed::IpVideoFeed(QString name, QUrl url, QObject *parent) :
    QObject(parent)
{
    setname(name);
    seturl(url);
}

QString IpVideoFeed::name() const
{
    return m_name;
}

QUrl IpVideoFeed::url() const
{
    return m_url;
}

void IpVideoFeed::setname(QString arg)
{
    if (m_name != arg) {
        m_name = arg;
        emit nameChanged(arg);
    }
}

void IpVideoFeed::seturl(QUrl arg)
{
    if (m_url != arg) {
        m_url = arg;
        emit urlChanged(arg);
    }
}
