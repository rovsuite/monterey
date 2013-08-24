#ifndef COMPASS_H
#define COMPASS_H

#include <QWidget>
#include <QtQuick>
#include <QQuickView>

class Compass : public QWidget
{
    Q_OBJECT
public:
    explicit Compass(QWidget *parent = 0);

    QWidget *container;
    
signals:
    
public slots:
    void onHeadingChange(int heading);
    void resetGraphics();

    void refreshPalette();  //call after setting palette

private slots:
    void initializeTicks();

private:
    QQuickView *viewer;
    QList<QObject*> tickList;
    QObject* currentHeadingReadout;
    int lastChange;
    int lastHeading;
    
};

#endif // COMPASS_H
