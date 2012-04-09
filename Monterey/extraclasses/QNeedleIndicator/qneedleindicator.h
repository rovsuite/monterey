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

/*! \mainpage QNeedleIndicator
 *
 * Simple needle indicator widget.
 *
 * \section install_sec Installation
 * Copy qneedleindicator.cpp and qneedleindicator.h file into your project.
 * Installation completed. :)
 * \bug
 * Too many things are hardcoded.
 *
 */

#ifndef QNEEDLEINDICATOR_H
#define QNEEDLEINDICATOR_H

#include <QtGui/QWidget>
#include <QDebug>
#include <QResizeEvent>
#include <QPainter>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QtGlobal>

class QNeedleIndicator : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal    min         READ getMinValue    WRITE setMinValue);
    Q_PROPERTY(qreal    max         READ getMaxValue    WRITE setMaxValue);
    Q_PROPERTY(qreal    value       READ getValue       WRITE setValue);
    Q_PROPERTY(qreal    gap_angle   READ getGapAngle    WRITE setGapAngle);
    Q_PROPERTY(bool     animated    READ isAnimated     WRITE setAnimated);
    Q_PROPERTY(int      majorTicks  READ getMajorTicks  WRITE setMajorTicks);
    Q_PROPERTY(int      minorTicks  READ getMinorTicks  WRITE setMinorTicks);
    Q_PROPERTY(QFont    labelFont   READ getLabelFont   WRITE setLabelFont);
    Q_PROPERTY(QFont    digitFont   READ getDigitFont   WRITE setDigitFont);
    Q_PROPERTY(QString  label       READ getLabel       WRITE setLabel);
    Q_PROPERTY(qreal    labelOffset READ getLabelOffset WRITE setLabelOffset);

public:
    /**
      * Creates QNeedleIndicator widget with default settings.
      */
    QNeedleIndicator(QWidget *parent = 0);

    /**
     * Set scale major ticks number. Default is 11 ticks. Don't set too many
     * ticks, because scale numbers will overlap.
     * @param ticks Major ticks number
     */
    void setMajorTicks(int ticks);

    /**
     * Set scale major ticks number.
     * @returns Major ticks number
     */
    int  getMajorTicks(void) { return (int)majorTicks; }

    /**
     * Set scale minor ticks number. Default is 4 ticks.
     * @param ticks Minor ticks number
     */
    void setMinorTicks(int ticks);

    /**
     * Get scale minor ticks number.
     * @returns Minor ticks number
     */
    int  getMinorTicks(void) { return (int)minorTicks; }

    /**
      * Set label font. Label is displayed below needle axis.
      * @param font Label font.
      */
    void setLabelFont(QFont font);

     /**
      * Get scale digit font.
      * @returns Scale digit font.
      */
    QFont getLabelFont(void) { return labelFont; }

    /**
      * Set scale digit font.
      * @param font Scale digit font.
      */
    void  setDigitFont(QFont font);

     /**
      * Get scale digit font.
      * @returns font Scale digit font.
      */
    QFont getDigitFont(void) { return digitFont; }

    /**
      * Set indicator range. All values passed to the indicator
      * will be coerced to this range. Scale will be drawn
      * to reflect this range too, so pay attention to set
      * sane values divisible by number of scale ticks.
      * @see setMinValue
      * @see setMaxValue
      * @param min Lowest possible value
      * @param max Maximum possible value.
      */
    void setRange(qreal min, qreal max);

    /**
      * Set indicator minumum level
      * @see setRange
      * @param max Minimum number shown by the indicator.
      */
    void  setMinValue(qreal min);

    /**
      * Get lower range boundary.
      * @returns Lower range boundary
      */
    qreal getMinValue(void) { return min; }

    /**
     * Set indicator maximum level
     * @see setRange
     * @param max Maximum number shown by the indicator.
     */
    void  setMaxValue(qreal max);

    /**
      * Get upper range boundary.
      * @returns Upper range boundary
      */
    qreal getMaxValue(void) { return max; }

    /**
      * Set animated property. If true, the needle will be animated
      * and will reach set value smoothly. If false, needle will be
      * drawn immediately.
      * @param anim Turn animation on when true and off when false.
      */
    void setAnimated(bool anim);

    /**
      * Returns animated property.
      * @returns True if needle is animated. False if not.
      */
    bool isAnimated(void);

    /**
      * Set label text. If label is empty, no label will be drawn.
      * Keep label short and sweet and to the point. It will not
      * autoscale to fit the meter.
      * @see setLabelFont
      * @param label Label text.
      */
    void setLabel(QString label);

    /**
      * Get label text.
      * returns Label text
      */
    QString getLabel(void) { return label; }

    /**
      * Set digit format. fmt is printf-like fmt string. Default is %.2f.
      * @parameter fmt Digit format.
      */
    void    setDigitFormat(QString fmt);

    /**
      * Get scale digit format string.
      * @returns Format string.
      */
    QString getDigitFormat(void) { return QString(scaleFormat); }

    /**
      * Set gap angle between first and last tick. Resulting scale will
      * have (360 - gap) degrees. Vale in degrees. Default is 90deg.
      * @param gap Scale gap angle in degrees
      */
    void  setGapAngle(qreal gap);

    /**
      * Get scale gap angle.
      * @returns Gap angle in degrees.
      */
    qreal getGapAngle(void) { return gap_angle; }

    /**
      * Set label offset. Label will be positioned between needle axis
      * and shield edge. 0 is needle level, 1 is edge level, 0.5 is between needle and edge,
      * 0.75 is closer to the edge, etc. Value must be between 0 and 1.
      * @param offset Label offset (0.0-1.0).
      */
    void  setLabelOffset(qreal offset);

    /**
      * Get label offset.
      * @returns Label offset (0.0 - 1.0)
      */
    qreal getLabelOffset(void) { return labelOffset/115.0; }

    /**
      * Get currently set value. This is NOT actual needle position during animation, but
      * final indicator value.
      * @returns Indicator value.
      */
    qreal getValue(void);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent (QPaintEvent  *event);

private slots:
    void animate(void);     // needle animation; fired from QTimer

public slots:
    /**
      * Set indicator value. If animated, this will also start needle animation. If
      * not animated, this will redraw the meter.
      * @param value New indicator value. Value will be coerced to min/max.
      */
    void setValue(qreal value);

private:
    qreal max;              // max indicator value (final on scale)
    qreal min;              // min indicator value (first on scale)
    qreal step;             // scale tick step
    qreal value;            // target indicator value
    qreal currValue;        // current value (needle position during animation)
    qreal majorTicks;       // Number of major scale ticks
    qreal minorTicks;       // Number of mainor scale ticks
    qreal rot_deg, rot_rad; // Scale number rotation step (deg & radians)
    qreal start_angle, stop_angle, gap_angle; // scale angles used for step/rotations/etc calculation
    QFont digitFont;
    QFont labelFont;
    QString label;
    QByteArray scaleFormat; // scale format string
    QTimer *timer;          // Animation timer
    int   animFrame;        // Animation frame coutner
    qreal valueDelta;       // Delta between current and target needle position value. Used in animation
    bool  animated;         // Animation flag. Animate needle when true
    qreal labelOffset;      // Label position between axis needle (when 0.0) and shield edge (when 1.0)

    void  drawBackground(void);     /*!< Draw background shield        */
    void  drawNeedle(void);         /*!< Draw needle                   */
    qreal value2angle(qreal value); /*!< Convert value to needle angle */
    void  startAnimation(void);     /*!< Start animation timer         */
};

#endif // QNEEDLEINDICATOR_H
