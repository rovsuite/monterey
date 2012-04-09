/***************************************************************************
 *   Copyright (C) 2010 by Tn                                              *
 *   thenobody@poczta.fm                                                   *
 *                                                                         *
 *   Foobar is free software: you can redistribute it and/or modify        *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of        *
 *   the License, or (at your option) any later version.                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.       *
 *                                                                         *
 ***************************************************************************/

#include "qneedleindicator.h"
#include <cmath>

QNeedleIndicator::QNeedleIndicator(QWidget *parent) : QWidget(parent)
{
    // Set some default values. FIX: shouldn't be hardcoded
    majorTicks = 11.0;
    minorTicks = 8.0;
    gap_angle   = 90;
    start_angle = 90 + gap_angle/2;
    stop_angle  = 360 - gap_angle;
    rot_deg = stop_angle/(majorTicks-1);
    rot_rad = (rot_deg/360.0)*2*M_PI;
    max     = 100;
    min     = 0;
    value   = 0;
    step    = max / (majorTicks-1);
    labelOffset = 115 * 0.6;
    digitFont = QFont("SansSerif", 14);
    digitFont.setStyleStrategy(QFont::PreferAntialias);
    labelFont = QFont("SansSerif", 18, QFont::DemiBold);
    labelFont.setStyleStrategy(QFont::PreferAntialias);
    label.clear();
    scaleFormat = "%.2f";
    animated = true;
    timer   = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(200,200);

}

void QNeedleIndicator::animate() {
    qreal p,s,f;
    animFrame++;
    // Divide value delta on pieces. Pieces length decrease.
    p = 10.0/((qreal)animFrame);
    s = 0;
    for(int i = 1; i <= 10; i++) s+=10.0/static_cast<qreal>(i); // Fix this crap
    f=p/s;
    currValue+=(valueDelta*f);
    if(animFrame >= 10) {
        timer->stop();
        currValue = value;  // lock the needle after animation on final position
    }
    update();
}

qreal QNeedleIndicator::getValue(void) {
    return value;
}

void QNeedleIndicator::startAnimation(void) {
    animFrame = 0;
    valueDelta = value-currValue;   // we need to move needle by valueDelta
    if( ! timer->isActive())        // fire timer; it will start moving the needle
        timer->start();             // from currValue to value
}

void QNeedleIndicator::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void QNeedleIndicator::paintEvent (QPaintEvent  *event) {
    drawBackground();
    drawNeedle();
    QWidget::paintEvent(event);
}

void QNeedleIndicator::setMajorTicks(int t) {
    majorTicks = static_cast<qreal>(t);
    step = (max-min)/(majorTicks-1);
    rot_deg = stop_angle/(majorTicks-1);
    rot_rad = (rot_deg/360.0)*2*M_PI;
    update();
}

void QNeedleIndicator::setMinorTicks(int t) {
    minorTicks = static_cast<qreal>(t);
    update();
}

void QNeedleIndicator::setDigitFont(QFont f) {
    digitFont = f;
    update();
}

void QNeedleIndicator::setLabelFont(QFont f) {
    labelFont = f;
    update();
}

void QNeedleIndicator::setAnimated(bool anim) {
    animated = anim;
}

bool QNeedleIndicator::isAnimated(void) {
    return animated;
}

void QNeedleIndicator::setValue(qreal v) {
    if(v > max) v = max;    // coerce
    if(v < min) v = min;
    if( animated ) {        // fire animation
        value = v;
        startAnimation();
    } else {                // instant update
        currValue = value = v;
        update();
    }
}

void QNeedleIndicator::setLabel(QString l) {
    label = l;
    update();
}

void QNeedleIndicator::setDigitFormat(QString format) {
    scaleFormat = format.toLatin1();
}

void QNeedleIndicator::setRange(qreal mi, qreal ma) {
    min  = mi;
    max  = ma;
    step = (max-min)/(majorTicks-1);
    update();
}

void QNeedleIndicator::setMinValue(qreal mi) {
    setRange(mi, max);
}

void QNeedleIndicator::setMaxValue(qreal ma) {
    setRange(min, ma);
}

void QNeedleIndicator::setGapAngle(qreal gap) {
    gap_angle   = gap;
    start_angle = 90 + gap_angle/2;
    stop_angle  = 360 - gap_angle;
    rot_deg = stop_angle/(majorTicks-1);
    rot_rad = (rot_deg/360.0)*2*M_PI;
    update();
}

void QNeedleIndicator::setLabelOffset(qreal offset) {
    Q_ASSERT(offset >= 0 && offset <= 1);
    if( offset < 0 ) offset = 0;
    if( offset > 1 ) offset = 1;
    labelOffset = 115*offset;
    update();
}

qreal QNeedleIndicator::value2angle(qreal val) {
    qreal intervalLength = max-min;
    val = val-min;

    if( val > intervalLength ) val = intervalLength;
    return start_angle + (val/intervalLength)*stop_angle;
}

void QNeedleIndicator::drawNeedle(void) {
    int side = qMin(width(), height());

    static const QPoint needle[5] = {
            QPoint(0,-2),
            QPoint(100, 0),
            QPoint(0,2),
            QPoint(-30,5),
            QPoint(-30,-5)
    };
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 256.0, side / 256.0);
    painter.save();
    painter.rotate(value2angle(currValue));
    painter.setBrush(Qt::red);
    painter.drawConvexPolygon(needle, 5);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPoint(0,0), 5,5);
    painter.restore();

}

void QNeedleIndicator::drawBackground(void) {
    int side = qMin(width(), height());
    /* Keep side size an even number by trunkating odd pixel */
    side &= ~0x01;

    QRadialGradient gradient;
    QPainter painter(this);
    QPen     pen(Qt::black);
             pen.setWidth(1);

    /* Initialize painter */
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 256.0, side / 256.0);
    painter.setPen(pen);
    /* Draw external circle */
    gradient = QRadialGradient (QPointF(-128,-128), 384, QPointF(-128,-128));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(pen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(QPoint(0,0),125,125);
    /* Draw inner circle */
    gradient = QRadialGradient(QPointF(128,128), 384, QPointF(128,128));
    gradient.setColorAt(0, QColor(224,224,224));
    gradient.setColorAt(1, QColor(28,28,28));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(QPoint(0,0),118,118);
    /* Draw inner shield */
    gradient = QRadialGradient (QPointF(-128,-128), 384, QPointF(-128,-128));
    gradient.setColorAt(0, QColor(255,255,255));
    gradient.setColorAt(1, QColor(224,224,224));
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(0,0), 115, 115);
    painter.setPen(pen);
    painter.setBrush(Qt::black);
    painter.drawPoint(0,0);

    int line = 10; /* FIX #1 */

    /* Draw scale majorTicks using coords rotation */
    painter.save();
    painter.setBrush(Qt::black);
    painter.rotate(start_angle);                /* initial angle (first tick) */
    painter.setBrush(QBrush(Qt::black));
    qreal t_rot = stop_angle/(minorTicks*(majorTicks-1)+majorTicks-1);
    for(int i = 0; i < (minorTicks)*(majorTicks-1)+majorTicks; i++) {
        if( minorTicks ) {
            if( i%(int)(minorTicks+1) == 0 )
                painter.drawLine(QPoint(105,0), QPoint(105-line, 0));
            else
                painter.drawLine(QPoint(105,0), QPoint(105-line/3, 0));
        } else {
            painter.drawLine(QPoint(105,0), QPoint(105-line, 0));
        }
        painter.rotate(t_rot);
    }
    painter.restore();

    /* Draw scale numbers using vector rotation */
    /* x' = xcos(a)-ysin(a)                     */
    /* y' = xsin(a)-ycos(a)                     */
    painter.save();
    qreal rotation = (start_angle/360)*2*M_PI;          /* Initial rotation */
    painter.setFont(digitFont);
    for(int i = 0; i < majorTicks; i++) {
        QPointF point((70*cos(rotation)), 70*sin(rotation));                           /* calculate digit coords      */
        QString value = QString().sprintf(scaleFormat.constData(), min+(qreal)i*step); /* convert digit to string     */
        QSize   size  = painter.fontMetrics().size(Qt::TextSingleLine, value);         /* get string size in px       */
        point.rx() -= size.width()/2;                                                  /* center-align string (horiz) */
        point.ry() += size.height()/4;                                                 /* center-align string (vert)  */
        painter.drawText(point, value);
        //painter.drawPoint(point);
        rotation+=rot_rad;                                                             /* go to next tick             */
    }
    painter.restore();

    /* Draw meter label */
    if( label.size() ) {
        painter.setFont(labelFont);
        QSize   size  = painter.fontMetrics().size(Qt::TextSingleLine, label);
        QPointF point;
        point.rx() -= size.width()/2;       /* center-align string (horiz) */
        point.ry() += size.height()/4 + labelOffset;  /* shift down, below needle    */
        painter.drawText(point, label);     /* blop!                       */
    }

}
