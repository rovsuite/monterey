#include "depthtape.h"
#include <QtQuick>
#include <QtDebug>

DepthTape::DepthTape(int maxDepth, QWidget *parent) :
    QWidget(parent)
{
    viewer = new QQuickView();
    container = QWidget::createWindowContainer(viewer);
    container->setMinimumSize(90, 100);
    container->setMaximumWidth(100);
    container->setFocusPolicy(Qt::TabFocus);

    viewer->setSource(QUrl("qrc:/qml/resources/Tape.qml"));
    viewer->rootContext()->setContextProperty("DepthTape", this);
    viewer->setResizeMode(QQuickView::SizeRootObjectToView);

    viewer->setColor(QColor(100,100,100));

    initializeTicks(maxDepth);

    connect(viewer, SIGNAL(heightChanged(int)), this, SLOT(heightChanged(int)));
}

void DepthTape::onDepthChange(double depth, QString units)
{
    if(lastDepth != depth)
    {
        QString depthString = QString::number(depth, 'f', 1);
        depthString.append(units);
        currentDepthReadout->setProperty("currentDepth", depthString);
        double newY = depth * -40.0;
        for(int i=0;i<tickList.count();i++)
        {
            tickList[i]->setProperty("y", tickList[i]->property("y").toInt() + (int)newY - (int)lastChange);
            tickBarList[i]->setProperty("y", tickList[i]->property("y").toInt() + (tickList[i]->property("height").toInt()/2));
        }
        lastChange = newY;
        lastDepth = depth;
    }
}

void DepthTape::setMaxDepth(int maxDepth)
{
    qDebug() << "DepthTape::setMaxDepth called";
    foreach(QObject* obj, tickList)
    {
        if(obj)
            obj->deleteLater();
    }
    tickList.clear();
    foreach(QObject* obj, tickBarList)
    {
        if(obj)
            obj->deleteLater();
    }
    tickBarList.clear();

    if(currentDepthReadout)
        currentDepthReadout->deleteLater();

    initializeTicks(maxDepth);
}

void DepthTape::heightChanged(int height)
{
    for(int i=0;i<tickList.count();i++)
    {
        tickList[i]->setProperty("y", i*40 + (height/2) - (tickList[i]->property("height").toInt()/2));
    }
    for(int i=0;i<tickBarList.count();i++)
    {
        tickBarList[i]->setProperty("y", i*40 + (height/2) - (tickBarList[i]->property("height").toInt()/2));
    }
    currentDepthReadout->setProperty("y", height/2 - currentDepthReadout->property("height").toInt()/2);
}

void DepthTape::initializeTicks(int maxDepth)
{
    QObject *verticalBar = viewer->rootObject()->findChild<QObject*>("verticalBar");

    for(int i = 0;i<maxDepth+1;i++)
    {
        //Add more ticks
        QQmlComponent component(viewer->engine(), QUrl("qrc:/qml/resources/Tick.qml"));
        QObject *myObject = component.create();
        QQuickItem *item = qobject_cast<QQuickItem*>(myObject);
        QQmlProperty::write(myObject, "parent", QVariant::fromValue<QObject*>(viewer->rootObject()));   //parents the new QObject* to the viewer so it will be painted
        QQmlEngine::setObjectOwnership(myObject, QQmlEngine::CppOwnership); //prevent garbage collection from deleting the object
        item->setProperty("x", 10);
        item->setProperty("y", i*40 + (viewer->height()/2) - item->height()/2);
        item->setProperty("text", i);
        tickList.append(myObject);
    }

    QQmlComponent currentDepthReadoutComponent(viewer->engine(), QUrl("qrc:/qml/resources/CurrentDepthReadout.qml"));
    currentDepthReadout = currentDepthReadoutComponent.create();
    QQuickItem *currentDepthReadoutItem = qobject_cast<QQuickItem*>(currentDepthReadout);
    QQmlProperty::write(currentDepthReadout, "parent", QVariant::fromValue<QObject*>(viewer->rootObject()));
    QQmlEngine::setObjectOwnership(currentDepthReadout, QQmlEngine::CppOwnership);
    //currentDepthReadoutItem->setX(tickList.last()->property("x").toInt() + tickList.last()->property("paintedWidth").toInt() + 20);
    currentDepthReadoutItem->setX(100 - currentDepthReadoutItem->width());
    currentDepthReadoutItem->setY(viewer->height()/2 - currentDepthReadoutItem->height()/2);

    verticalBar->setProperty("x", currentDepthReadoutItem->x() + currentDepthReadoutItem->width() - 2);
    verticalBar->setProperty("height", container->height());
    viewer->rootObject()->setProperty("width", verticalBar->property("x").toInt());
    container->setMaximumWidth(viewer->rootObject()->property("width").toInt());

    for(int i = 0;i<maxDepth+1;i++)
    {
        QQmlComponent tickBarComponent(viewer->engine(), QUrl("qrc:/qml/resources/TickBar.qml"));
        QObject *tickBarObject = tickBarComponent.create();
        QQuickItem *tickBarItem = qobject_cast<QQuickItem*>(tickBarObject);
        QQmlProperty::write(tickBarObject, "parent", QVariant::fromValue<QObject*>(viewer->rootObject()));
        QQmlEngine::setObjectOwnership(tickBarObject, QQmlEngine::CppOwnership);
        tickBarItem->setProperty("x", tickList[i]->property("x").toInt() + tickList[i]->property("paintedWidth").toInt() + 5);
        tickBarItem->setProperty("y", tickList[i]->property("y").toInt() - (tickList[i]->property("height").toInt()/2));
        if(verticalBar)
        {
            tickBarItem->setProperty("width", viewer->rootObject()->property("width").toInt() - tickBarItem->x());
        }
        else
        {
            tickBarItem->setProperty("width", viewer->width() - tickBarItem->x());
            qDebug() << "No vertical bar found!  Falling back to default.";
        }
        tickBarList.append(tickBarObject);
    }

    lastChange = 0;
    lastDepth = 0;
    viewer->show();
}

