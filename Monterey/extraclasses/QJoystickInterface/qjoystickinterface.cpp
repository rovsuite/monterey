#include "qjoystickinterface.h"
#include <QtGlobal>
#include <QList>

QJoystickInterface::QJoystickInterface()
{
    enablePolling(true);
    mTimer.start(POLL_INTERVAL);
    mBilinearEnabled = false;
    mBilinearConstant = 1;
}

QJoystickInterface::~QJoystickInterface()
{

}

bool QJoystickInterface::currentJoystick(const int id)
{
    enablePolling(false);
    bool ret = false;
    if(0 <= id && id < numJoysticks())
    {
        mCurrentId = id;
        init(); //update js model
        ret = true;
    }

    enablePolling(true);

    return ret;
}

int QJoystickInterface::currentJoystick() const
{
    return mCurrentId;
}

void QJoystickInterface::bilinearEnable(bool enable)
{
    mBilinearEnabled = enable;
}

bool QJoystickInterface::bilinearEnabled() const
{
    return mBilinearEnabled;
}

void QJoystickInterface::bilinearConstant(double k)
{
    mBilinearConstant = k;
}

double QJoystickInterface::bilinearConstant() const
{
    return mBilinearConstant;
}

bool QJoystickInterface::deadzone(int axisId, int size)
{
    bool ret = false;
    if(0 <= axisId && axisId < mDeadzones.count() &&
            0 <= size && size <= 32767)
    {
        mDeadzones[axisId] = size;
        ret = true;
    }

    return ret;
}

int QJoystickInterface::deadzone(int axisId) const
{
    int ret = -1;
    if(0 <= axisId && axisId < mDeadzones.count())
    {
        ret = mDeadzones[axisId];
    }

    return ret;
}

void QJoystickInterface::init()
{
    enablePolling(false);
    //Reset axes information
    mDeadzones.clear();
    mAxesCurrent.clear();
    for(int i=0; i<numAxes(); i++)
    {
        mDeadzones.append(0);
        mAxesCurrent.append(0);
    }

    //Reset button information
    mButtonsCurrent.clear();
    mButtonsPrevious.clear();
    mButtonsToggled.clear();
    for(int i=0; i<numButtons(); i++)
    {
        mButtonsCurrent.append(false);
        mButtonsPrevious.append(false);
        mButtonsToggled.append(false);
    }

    //Reset hat information
    mHatsCurrent.clear();
    mHatsPrevious.clear();
    mHatsToggled.clear();
    for(int i=0; i<numHats(); i++)
    {
        QList<bool> tempCur;
        QList<bool> tempPrev;
        QList<bool> tempTog;
        for(int j=0; j<9; j++)
        {
            tempCur.append(true);
            tempPrev.append(false);
            tempTog.append(true);
        }
        mHatsCurrent.append(tempCur);
        mHatsPrevious.append(tempPrev);
        mHatsToggled.append(tempTog);
    }
    enablePolling(true);
}

void QJoystickInterface::processDeadzones()
{
    enablePolling(false);
    for(int i=0; i<mAxesCurrent.count(); i++)
    {
        int dz = mDeadzones[i];
        if(qAbs(mAxesCurrent[i]) <= dz)
        {
            mAxesCurrent[i] = 0;
        }
        else
        {
            const int posMax = 32767;
            const int negMax = -32768;
            double slope;
            int b;
            if(mAxesCurrent[i] > 0)
            {
                slope = (double)posMax/(posMax-dz);
                b = -1 * slope * dz;
            }
            else
            {
                slope = (double)negMax/(negMax+dz);
                b = slope * dz;
            }

            //Calculate the new value
            mAxesCurrent[i] = (int)(mAxesCurrent[i] * slope) + b;
        }
    }
    enablePolling(true);
}

void QJoystickInterface::processBilinear()
{
    enablePolling(false);
    if(bilinearEnabled())
    {
        Q_ASSERT(mBilinearConstant > 0);
        double bilinearThreshold = 1.0/mBilinearConstant;
        const int min = -32768;
        const int max = 32767;
        for(int i=0; i<mAxesCurrent.count(); i++)
        {
            Q_ASSERT(min <= mAxesCurrent[i]);
            Q_ASSERT(mAxesCurrent[i] <= max);
            int cur = mAxesCurrent[i];
            //If the stick is within the range
            if(bilinearThreshold * min <= cur && cur <= bilinearThreshold * max)
            {
                mAxesCurrent[i] = (mAxesCurrent[i] / mBilinearConstant);
            }
            //If the stick is in the upper section of the range
            else if((bilinearThreshold * max) < cur && cur <= max)
            {
                mAxesCurrent[i] = (mBilinearConstant * cur) +
                    ((mBilinearConstant * min) + max) + 2;
            }
            //If the stick is in the lower section of the range
            else if(min <= cur && cur < (bilinearThreshold * min))
            {
                mAxesCurrent[i] = (mBilinearConstant * cur) +
                    ((mBilinearConstant * max) + min) + 1;
            }
        }
    }
    enablePolling(true);
}

void QJoystickInterface::enablePolling(bool enable)
{
   if(enable)
   {
       connect(&mTimer, SIGNAL(timeout()), this, SLOT(getData()));
   }
   else
   {
       disconnect(&mTimer, SIGNAL(timeout()), this, SLOT(getData()));
   }
}

const QList<int>& QJoystickInterface::axisStates() const
{
    return mAxesCurrent;
}

const QList<bool>& QJoystickInterface::buttonStates() const
{
    return mButtonsCurrent;
}

QList<int> QJoystickInterface::hatStates()
{
    enablePolling(false);
    QList<int> ret;
    //This returns the first pressed direction for
    //each hat (if multiple directions per hat are
    //pressed.
    for(int i=0; i<mHatsCurrent.count(); i++)
    {
        for(int j=0; j<mHatsCurrent[i].count(); j++)
        {
            if(mHatsCurrent[i][j])
            {
                ret.append(j);
                break;
            }
        }
    }
    enablePolling(true);

    return ret;
}

bool QJoystickInterface::joystickAttached() const
{
    return (0 < numJoysticks());
}
