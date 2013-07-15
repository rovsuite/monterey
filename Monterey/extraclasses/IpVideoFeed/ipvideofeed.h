#ifndef IPVIDEOFEED_H
#define IPVIDEOFEED_H

#include <QObject>
#include <QUrl>
#include <QString>

class IpVideoFeed : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setname NOTIFY nameChanged)
    Q_PROPERTY(QUrl url READ url WRITE seturl NOTIFY urlChanged)

    QString m_name;

    QUrl m_url;

public:
    explicit IpVideoFeed(QString name, QUrl url, QObject *parent = 0);

    QString name() const;

    QUrl url() const;

signals:

    void nameChanged(QString arg);

    void urlChanged(QUrl arg);

public slots:

    void setname(QString arg);
    void seturl(QUrl arg);
};

#endif // IPVIDEOFEED_H
