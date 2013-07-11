#ifndef DEPTHTAPE_H
#define DEPTHTAPE_H

#include <QWidget>
#include <QtQuick>
#include <QQuickView>

class DepthTape : public QWidget
{
    Q_OBJECT
public:
    explicit DepthTape(int maxDepth, QWidget *parent = 0);

    QWidget *container;
    
signals:
    
public slots:
    void onDepthChange(double depth);
    void setMaxDepth(int maxDepth);

    void heightChanged(int height);

private:
    QQuickView *viewer;
    QList<QObject*> tickList;
    QList<QObject*> tickBarList;
    QObject* currentDepthReadout;
    
};

#endif // DEPTHTAPE_H
