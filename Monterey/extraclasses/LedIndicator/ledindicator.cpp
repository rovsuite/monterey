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

    refreshPalette();
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

void LedIndicator::refreshPalette()
{
    QPalette p = this->palette();

    viewer->setColor(p.window().color());
    viewer->rootObject()->setProperty("textColor", p.text().color());
    viewer->rootObject()->setProperty("borderColor", p.text().color());

    QColor backgroundColor = p.highlight().color();
    backgroundColor.setAlpha(90);
    viewer->rootObject()->setProperty("backgroundColor", backgroundColor);

    QColor titleColor = p.highlight().color();
    viewer->rootObject()->setProperty("titleColor", titleColor);
}
