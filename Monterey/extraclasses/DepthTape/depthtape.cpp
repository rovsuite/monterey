#include "depthtape.h"
#include <QtQuick>
#include <QtDebug>

DepthTape::DepthTape(int maxDepth, QWidget *parent) :
    QWidget(parent)
{
    viewer = new QQuickView();
    container = QWidget::createWindowContainer(viewer);
    container->setFixedWidth(70);
    container->setFocusPolicy(Qt::TabFocus);

    viewer->setSource(QUrl("qrc:/qml/resources/DepthTape.qml"));
    viewer->rootContext()->setContextProperty("DepthTape", this);
    viewer->setResizeMode(QQuickView::SizeRootObjectToView);

    viewer->setColor(QColor(53,53,53));
    lastMaxDepth = maxDepth;

    initializeTicks(maxDepth);

    connect(viewer, SIGNAL(heightChanged(int)), this, SLOT(resetGraphics()));
    connect(viewer, SIGNAL(widthChanged(int)), this, SLOT(resetGraphics()));
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
        }
        lastChange = newY;
        lastDepth = depth;
    }
}

void DepthTape::setMaxDepth(int maxDepth)
{
    foreach(QObject* obj, tickList)
    {
        if(obj)
            obj->deleteLater();
    }
    tickList.clear();

    if(currentDepthReadout)
        currentDepthReadout->deleteLater();

    lastMaxDepth = maxDepth;
    initializeTicks(maxDepth);
}

void DepthTape::resetGraphics()
{
    foreach(QObject *obj, tickList)
    {
        if(obj)
            delete obj;
    }
    if(currentDepthReadout)
        delete currentDepthReadout;
    tickList.clear();
    initializeTicks(lastMaxDepth);
}

void DepthTape::refreshPalette()
{
    QPalette p = this->palette();
    viewer->setColor(p.window().color());
    viewer->rootObject()->setProperty("color", p.base().color());
    viewer->rootObject()->setProperty("borderColor", p.highlight().color());

    for(int i =0; i<tickList.count();i++)
    {
        tickList[i]->setProperty("colorToDisplay", p.highlight().color());
    }

    currentDepthReadout->setProperty("backgroundColor", p.base().color());
    currentDepthReadout->setProperty("borderColor", p.highlight().color());
    currentDepthReadout->setProperty("textColor", p.text().color());
}

void DepthTape::initializeTicks(int maxDepth)
{
    QQmlComponent currentDepthReadoutComponent(viewer->engine(), QUrl("qrc:/qml/resources/CurrentDepthReadout.qml"));
    currentDepthReadout = currentDepthReadoutComponent.create();
    QQuickItem *currentDepthReadoutItem = qobject_cast<QQuickItem*>(currentDepthReadout);
    QQmlProperty::write(currentDepthReadout, "parent", QVariant::fromValue<QObject*>(viewer->rootObject()));
    QQmlEngine::setObjectOwnership(currentDepthReadout, QQmlEngine::CppOwnership);
    int width = viewer->width();
    currentDepthReadoutItem->setX(width - currentDepthReadoutItem->width() + 2);
    currentDepthReadoutItem->setY(viewer->height()/2 - currentDepthReadoutItem->height()/2);
    currentDepthReadoutItem->setZ(5);

    for(int i = 0;i<maxDepth+1;i++)
    {
        QQmlComponent tickComponent(viewer->engine(), QUrl("qrc:/qml/resources/DepthTick.qml"));
        QObject *tickObject = tickComponent.create();
        QQuickItem *tickItem = qobject_cast<QQuickItem*>(tickObject);
        QQmlProperty::write(tickObject, "parent", QVariant::fromValue<QObject*>(viewer->rootObject()));
        QQmlEngine::setObjectOwnership(tickObject, QQmlEngine::CppOwnership);
        tickItem->setProperty("textToDisplay", QString::number(i));
        tickItem->setProperty("x", 15 - tickItem->property("textX").toInt());
        tickItem->setProperty("y", i*40 + (viewer->height()/2) - tickItem->height()/2);
        tickItem->setProperty("width", viewer->width() - tickItem->x());
        tickList.append(tickObject);
    }

    lastChange = 0;
    lastDepth = 0;
    refreshPalette();
    //viewer->show();
}
