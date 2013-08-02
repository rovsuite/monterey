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
    Q_PROPERTY(bool autoGenerate READ autoGenerate WRITE setAutoGenerate NOTIFY autoGenerateChanged)

    QString m_name;

    QUrl m_url;

    bool m_autoGenerate;

public:
    explicit IpVideoFeed(QString name, QUrl url, bool autoGenerate, QObject *parent = 0);

    QString name() const;

    QUrl url() const;

    bool autoGenerate() const;

signals:

    void nameChanged(QString arg);

    void urlChanged(QUrl arg);

    void autoGenerateChanged(bool arg);

public slots:

    void setname(QString arg);
    void seturl(QUrl arg);
    void setAutoGenerate(bool arg);
};

#endif // IPVIDEOFEED_H
