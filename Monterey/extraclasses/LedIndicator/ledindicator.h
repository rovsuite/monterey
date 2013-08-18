#ifndef LEDINDICATOR_H
#define LEDINDICATOR_H

#include <QWidget>
#include <QtQuick>
#include <QQuickView>

class LedIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit LedIndicator(QWidget *parent = 0);
    
    QWidget *container;
signals:
    
public slots:
    void setIndicatorTitle(QString title);
    QString indicatorTitle();

    void setStatus(bool on);
    bool status();

private:
    QQuickView *viewer;
    
};

#endif // LEDINDICATOR_H
