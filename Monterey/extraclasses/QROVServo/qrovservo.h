//This class is used to hold ROV servo values
//Written by Chris Konstad (02/24/2012)

//There is no warranty provided with this simple library
//Feel free to distribute, modify, use, etc this library, just make sure
//to give credit to the original author
#ifndef QROVSERVO_H
#define QROVSERVO_H

#include <QObject>
#include <QString>

class QROVServo : public QObject
{
    Q_OBJECT
public:
    explicit QROVServo(QObject *parent = 0);

signals:
    void valueChanged(double currentPercent);
    void valueChanged(int currentValue);

public slots:
    QString getName();
    void setName(QString n);
    int getValue();
    void setValue(int v);
    int getMin();
    void setMin(int m);
    int getMax();
    void setMax(int m);
    double getPercentRotation();

private:
    QString name;
    int value;
    int min;
    int max;
    int lastValue;
};

#endif // QROVSERVO_H
