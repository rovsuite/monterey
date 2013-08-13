#include "compass.h"

Compass::Compass(QWidget *parent) :
    QWidget(parent)
{
    viewer = new QQuickView();
    container = QWidget::createWindowContainer(viewer);
    container->setMinimumSize(100, 35);
    container->setMaximumHeight(35);
    container->setFocusPolicy(Qt::TabFocus);

    viewer->setSource(QUrl("qrc:/qml/resources/Compass.qml"));
    viewer->rootContext()->setContextProperty("Compass", this);
    viewer->setResizeMode(QQuickView::SizeRootObjectToView);

    initializeTicks();

    connect(viewer, SIGNAL(heightChanged(int)), this, SLOT(resetGraphics()));
    connect(viewer, SIGNAL(widthChanged(int)), this, SLOT(resetGraphics()));
}

void Compass::onHeadingChange(int heading)
{
    if(lastHeading != heading)
    {
        currentHeadingReadout->setProperty("currentHeading", QString::number(heading));
        int newX = heading * 10;   //10 = multiplier, DIFFERENT FROM OTHER MULTIPLIER BECAUSE OF MATH USED
        for(int i=0; i<tickList.count(); i++)
        {
            tickList[i]->setProperty("x", tickList[i]->property("x").toInt() - newX + lastChange);
        }
        lastChange = newX;
        lastHeading = heading;
    }

}

void Compass::resetGraphics()
{
    foreach(QObject *obj, tickList)
    {
        if(obj)
            delete obj;
    }
    if(currentHeadingReadout)
        delete currentHeadingReadout;
    tickList.clear();
    initializeTicks();
}

void Compass::initializeTicks()
{
    qDebug() << "Compass::initializeTicks";
    //Ticks
    int multiplier = 100;
    int numberOfExtraTicksPerSide = viewer->width()/multiplier;
    int numberOfTicks = 36 + 2*numberOfExtraTicksPerSide;
    for(int i=0; i<numberOfTicks;i++)
    {
        QQmlComponent component(viewer->engine(), QUrl("qrc:/qml/resources/CompassTick.qml"));
        QObject *myObject = component.create();
        QQuickItem *item = qobject_cast<QQuickItem*>(myObject);
        QQmlProperty::write(myObject, "parent", QVariant::fromValue<QObject*>(viewer->rootObject()));
        QQmlEngine::setObjectOwnership(myObject, QQmlEngine::CppOwnership);
        item->setProperty("y", viewer->height() - item->height());

        int indexOfZeroTick = numberOfExtraTicksPerSide + 1;

        int x = viewer->width()/2 - item->width()/2;
        x = x + multiplier * (i - indexOfZeroTick);
        item->setProperty("x", x);

        if( i == indexOfZeroTick)   //the centered "0" tick
        {
            item->setProperty("textToDisplay", QString::number(0));
        }
        else if( i > indexOfZeroTick && i <= indexOfZeroTick + 35)   //"normal" range of ticks
        {
            item->setProperty("textToDisplay", QString::number((i - indexOfZeroTick) * 10));
        }
        else if (i > indexOfZeroTick && i > indexOfZeroTick + 35)
        {
            int indexToDisplay = i - (indexOfZeroTick + 36);
            item->setProperty("textToDisplay", QString::number(indexToDisplay * 10));
        }
        else if (i < indexOfZeroTick)
        {
            int indexToDisplay = indexOfZeroTick - i;
            item->setProperty("textToDisplay", QString::number((36 - indexToDisplay) * 10));
        }
        else
        {
            qWarning() << "Error with CompassTick index!";
        }

        tickList.append(myObject);
    }

    //Current heading readout
    QQmlComponent currentHeadingReadoutComponent(viewer->engine(), QUrl("qrc:/qml/resources/CompassReadout.qml"));
    currentHeadingReadout = currentHeadingReadoutComponent.create();
    QQuickItem *currentHeadingReadoutItem = qobject_cast<QQuickItem*>(currentHeadingReadout);
    QQmlProperty::write(currentHeadingReadout, "parent", QVariant::fromValue<QObject*>(viewer->rootObject()));
    QQmlEngine::setObjectOwnership(currentHeadingReadout, QQmlEngine::CppOwnership);
    currentHeadingReadoutItem->setWidth(50);
    currentHeadingReadoutItem->setHeight(30);
    currentHeadingReadoutItem->setX(viewer->width()/2 - currentHeadingReadoutItem->width()/2);
    currentHeadingReadoutItem->setY(viewer->height() - currentHeadingReadoutItem->height() + 5);    //+5 for the border
    lastChange = 0;
    lastHeading = 0;
    viewer->show();
}
