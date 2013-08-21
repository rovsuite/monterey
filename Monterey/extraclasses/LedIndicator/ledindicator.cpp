#include "ledindicator.h"

LedIndicator::LedIndicator(QWidget *parent) :
    QWidget(parent)
{
    viewer = new QQuickView();
    container = QWidget::createWindowContainer(viewer);
    container->setFocusPolicy(Qt::TabFocus);

    viewer->setSource(QUrl("qrc:/qml/resources/LedIndicator.qml"));
    viewer->rootContext()->setContextProperty("LedIndicator", this);
    viewer->setResizeMode(QQuickView::SizeRootObjectToView);

    viewer->setColor(qApp->palette().window().color());
}

void LedIndicator::setIndicatorTitle(QString title)
{
    viewer->rootObject()->setProperty("indicatorTitle", title);
}

QString LedIndicator::indicatorTitle()
{
    return viewer->rootObject()->property("indicatorTitle").toString();
}

void LedIndicator::setStatus(bool on)
{
    viewer->rootObject()->setProperty("isStatusOn", on);
}

bool LedIndicator::status()
{
    return viewer->rootObject()->property("isStatusOn").toBool();
}
