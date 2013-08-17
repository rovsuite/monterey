#include "ipvideofeed.h"

IpVideoFeed::IpVideoFeed(QString name, QUrl url, bool autoGenerate, QObject *parent) :
    QObject(parent)
{
    setname(name);
    seturl(url);
    setAutoGenerate(autoGenerate);
}

QString IpVideoFeed::name() const
{
    return m_name;
}

QUrl IpVideoFeed::url() const
{
    return m_url;
}

bool IpVideoFeed::autoGenerate() const
{
    return m_autoGenerate;
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
    QString checkForHttp = arg.toString();
    if(!checkForHttp.contains("http://", Qt::CaseInsensitive))
    {
        checkForHttp.prepend("http://");
        arg.setUrl(checkForHttp);
    }
    if (m_url != arg) {
        m_url = arg;
        emit urlChanged(arg);
    }
}

void IpVideoFeed::setAutoGenerate(bool arg)
{
    if (m_autoGenerate != arg) {
        m_autoGenerate = arg;
        emit autoGenerateChanged(arg);
    }
}
