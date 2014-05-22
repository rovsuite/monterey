//This class is used to hold ROV motor values
//Written by Chris Konstad (02/24/2012)

#ifndef QROVMOTOR_H
#define QROVMOTOR_H

#include <QObject>

class QROVMotor : public QObject
{
    Q_OBJECT
public:
    explicit QROVMotor(QObject *parent = 0);

signals:

public slots:
    void init(int minimum, int maximum, int inputValue);

    void setValue(int input);
    int getValue();
    void setMinimum(int minimum);
    int getMinimum();
    void setMaximum(int maximum);
    int getMaximum();
    double getPercentThrust();

private:
    int min;
    int max;
    int value;

};

#endif // QROVMOTOR_H
