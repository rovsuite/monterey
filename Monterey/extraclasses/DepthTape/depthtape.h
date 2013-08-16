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
    void onDepthChange(double depth, QString units);
    void setMaxDepth(int maxDepth);
    void resetGraphics();

private slots:
    void initializeTicks(int maxDepth);

private:
    QQuickView *viewer;
    QList<QObject*> tickList;
    QObject* currentDepthReadout;
    double lastChange;
    double lastDepth;
    int lastMaxDepth;
    
};

#endif // DEPTHTAPE_H
