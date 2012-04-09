#include "qvectordrive2.h"

QVectorDrive2::QVectorDrive2(QObject *parent) :
    QObject(parent)
{
}

QVectorDrive2::~QVectorDrive2()
{
    vectorValues.clear();
    this->deleteLater();
}

int QVectorDrive2::mapInt(int input, int inMin, int inMax, int outMin, int outMax)
{
    int output = (input - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    return output;
}

int QVectorDrive2::minInt(int a, int b)
{
    if(a > b)
    {
        return b;
    }
    else
    {
        return a;
    }
}

int QVectorDrive2::maxInt(int a, int b)
{
    if(a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

int QVectorDrive2::constrainInt(int a, int min, int max)
{
    if(min < a && a < max)
    {
        return a;
    }
    else if(a <= min && a < max)
    {
        return min;
    }
    else if(min < a && max <= a)
    {
        return max;
    }
}

double QVectorDrive2::maxDouble(double a, double b)
{
    if(a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

void QVectorDrive2::initVector(int _vectorMin, int _vectorMax, int _xDeadzone, int _yDeadzone, int _zDeadzone)
{
    vectorMin = _vectorMin;
    vectorMax = _vectorMax;

    xDeadzone = _xDeadzone;
    yDeadzone = _yDeadzone;
    zDeadzone = _zDeadzone;

    vectorValues.clear();

    vectorValues.append(0);
    vectorValues.append(0);
    vectorValues.append(0);
    vectorValues.append(0);
    vectorValues.append(0);
    vectorValues.append(0);

    motorDoubles.clear();

    motorDoubles.append(0);
    motorDoubles.append(0);
    motorDoubles.append(0);
    motorDoubles.append(0);

}

int QVectorDrive2::getVectorValue(int listIndex)
{
    return vectorValues[listIndex];
}

void QVectorDrive2::vectorMath(int xInput, int yInput, int zInput, int vInput, bool inverted)
{
    if(inverted)
    {
        xVal = mapInt(xInput, -32768, 32767, 32767, -32768);
        yVal = mapInt(yInput, -32768, 32767, 32767, -32768);
        zVal = mapInt(zInput, -32768, 32767, 32767, -32768);
        vVal = vInput;
    }
    else
    {
        xVal = xInput;
        yVal = yInput;
        zVal = -zInput/2;   //fix rotational sensitivity?
        vVal = vInput;
    }

    if(qAbs(xVal) < xDeadzone)
    {
        xVal = 0;
    }

    if(qAbs(yVal) < yDeadzone)
    {
        yVal = 0;
    }

    if(qAbs(zVal) < zDeadzone)
    {
        zVal = 0;
    }

    vectorType = 1;

    switch(vectorType)
    {
    case 1: //Rolf Konstad's vector formula
        //Percent calculations
        double xPercent = (100 / (32767 - (double)xDeadzone)) * (xVal - (double)xDeadzone);
        double yPercent = -(100 / (32767 - (double)yDeadzone)) * (yVal - (double)yDeadzone);
        double zPercent = (100 / (32767 - (double)zDeadzone)) * (zVal - (double)zDeadzone);

        double maxInput = maxDouble(qAbs(xPercent), qAbs(yPercent));
        maxInput = maxDouble(qAbs(maxInput), qAbs(zPercent));

        //Motor calculations
        motorDoubles[0] = -xPercent - yPercent + zPercent;
        motorDoubles[1] = xPercent - yPercent - zPercent;
        motorDoubles[2] = xPercent + yPercent + zPercent;
        motorDoubles[3] = -xPercent + yPercent - zPercent;
        double maxMotor = maxDouble(qAbs(motorDoubles[0]), qAbs(motorDoubles[1]));
        maxMotor = maxDouble(qAbs(maxMotor), qAbs(motorDoubles[2]));
        maxMotor = maxDouble(qAbs(maxMotor), qAbs(motorDoubles[3]));

        //Normalize the values so that no motor outputs over 100% thrust
        motorDoubles[0] = motorDoubles[0] * (maxInput/maxMotor);
        motorDoubles[1] = motorDoubles[1] * (maxInput/maxMotor);
        motorDoubles[2] = motorDoubles[2] * (maxInput/maxMotor);
        motorDoubles[3] = motorDoubles[3] * (maxInput/maxMotor);

        //Create final motor command and convert double to int
        for(int i=0; i < motorDoubles.count(); i++)
        {
            vectorValues[i] = (int)motorDoubles[i];
            vectorValues[i] = vectorValues[i] * (32767/100);
        }
        vectorValues[4] = vVal;
        vectorValues[5] = vVal;

        break;
    }

    //map values to create final output
    vectorValues[0] = mapInt(vectorValues[0], 32767, -32768, vectorMin, vectorMax);
    vectorValues[1] = mapInt(vectorValues[1], 32767, -32768, vectorMin, vectorMax);
    vectorValues[2] = mapInt(vectorValues[2], 32767, -32768, vectorMin, vectorMax);
    vectorValues[3] = mapInt(vectorValues[3], 32767, -32768, vectorMin, vectorMax);
    vectorValues[4] = mapInt(vectorValues[4], -32768, 32767, vectorMin, vectorMax);
    vectorValues[5] = mapInt(vectorValues[5], -32768, 32767, vectorMin, vectorMax);

    for(int i=0; i < vectorValues.count(); i++)    //constrain values
    {
        vectorValues[i] = constrainInt(vectorValues[i], vectorMin, vectorMax);
    }
}
