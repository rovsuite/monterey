#ifndef QVECTORDRIVE2_H
#define QVECTORDRIVE2_H

/*
  Vector math created by Rolf Konstad.
  Library made by Chris Konstad.

  For an ROV with 4 horizontal motors and
  2 vertical motors
*/

#include <QObject>
#include <QList>
#include <qmath.h>
#include <math.h>

class QVectorDrive2 : public QObject
{
    Q_OBJECT
public:
    explicit QVectorDrive2(QObject *parent = 0);
    ~QVectorDrive2();

signals:

public slots:
    void initVector(int _vectorMin, int _vectorMax, int _xDeadzone, int _yDeadzone, int _zDeadzone);  //init vector-wide parameters

    void vectorMath(int xInput, int yInput, int zInput, int vInput, bool inverted);    //set vectorValues, writes to QList

    int getVectorValue(int listIndex);

private:
    QList<int> vectorValues;    //FL=0,FR=1,BR=2,BL=3 final values

    int vectorMax;  //maximum value (full forward)
    int vectorMin;  //minimum value (full reverse)

    int xVal;
    int yVal;
    int zVal;
    int vVal;   //vertical Value

    int xDeadzone;
    int yDeadzone;
    int zDeadzone;
    int vectorType;

    QList<double> motorDoubles;

    int mapInt(int input, int inMin, int inMax, int outMin, int outMax);    //taken from Arduino.cc
    int minInt(int a, int b);
    int maxInt(int a, int b);
    int constrainInt(int a, int min, int max);

    double maxDouble(double a, double b);
};

#endif // QVECTORDRIVE2_H
