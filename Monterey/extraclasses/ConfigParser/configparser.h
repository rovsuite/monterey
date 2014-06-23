#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <QObject>
#include "qrov.h"

class ConfigParser : public QObject
{
    Q_OBJECT
public:
    explicit ConfigParser(const QString& file, QObject *parent = 0);

signals:

public slots:
    bool parseRov(QROV& rov) const;
    void setFile(const QString& filename);

private:
    QByteArray readFile() const;

    QString mFile;

};

#endif // CONFIGPARSER_H
