#ifndef QROVSENSOR_H
#define QROVSENSOR_H

//Written by Chris Konstad
//March 2012

#include <QObject>
#include <QString>

class QROVSensor : public QObject
{
    Q_OBJECT
public:
    explicit QROVSensor(QObject *parent = 0);

signals:
    void thresholdReached();

public slots:
    void setMin(double m);  //!< Set the minimum value of the range
    double getMin();    //!< Get the minimum value of the range

    void setMax(double m);  //!< Set the maximum value of the range
    double getMax();    //!< Get the maximum value of the range

    void setValue(double v);    //!< Set the sensor's value
    double getValue();  //!< Retrieve the sensor's value

    void setThreshold(double t);    //!< Set the warning threshold
    double getThreshold();  //!< Get the warning threshold

    double getPercent();    //!< Return the percent reading of the sensor

    void setWarnAbove(bool b);  //!< Set the sensor to warn when the value goes to the threshold or above
    bool getWarnAbove();

    void setWarnBelow(bool b);  //!< Set the sensor to warn when the value goes to the threshold or below
    bool getWarnBelow();

    void setUnits(QString u);
    QString getUnits();

    void setName(QString n);
    QString getName();

 private:
    double min;
    double max;
    double val;
    double threshold;
    bool warnAbove; //true = warn when above threshold, false = warn when below threshold
    QString units;
    QString name;
};

#endif // QROVSENSOR_H
