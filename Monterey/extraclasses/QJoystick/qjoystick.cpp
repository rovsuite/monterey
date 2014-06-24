#include "qjoystick.h"

QJoystick::QJoystick()
{
    // Sure, we're only using the Joystick, but SDL doesn't work if video isn't initialised
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    mJoystick = NULL;   //js not opened yet
}

QJoystick::~QJoystick()
{
    SDL_JoystickClose(m_joystick);
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
}

void QJoystick::init()
{
    Q_ASSERT(mCurrentId >= 0);
    Q_ASSERT(mCurrentId < numJoysticks());
    
    mJoystick  = SDL_JoystickOpen(mCurrentId);
    QJoystickInterface::init();
}

const QString& QJoystick::name() const
{
    return QString(SDL_JoystickName(mCurrentId));
}

int QJoystick::numAxes() const
{
    int ret = -1;
    if(mJoystick != NULL)
    {
        ret = SDL_JoystickNumAxes(mJoystick);
    }

    return ret;
}

int QJoystick::numButtons() const
{
    int ret = -1;
    if(mJoystick != NULL)
    {
        ret = SDL_JoystickNumButtons(mJoystick);
    }

    return ret;
}

int QJoystick::numHats() const
{
    int ret = -1;
    if(mJoystick != NULL)
    {
        ret = SDL_JoystickNumHats(mJoystick);
    }

    return ret;
}

int QJoystick::reenumerateDevices()
{
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);   //Quit the subsystem
    SDL_Init(SDL_INIT_JOYSTICK);    //Re-open the subsystem (also reenumerate devices)

    return SDL_NumJoysticks();  //return the number of attached joysticks
}

int QJoystick::numJoysticks() const
{
     return SDL_NumJoysticks();
}

void QJoystick::getData()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    for(int i=0;i<getNumAxes();i++)
    {
        mAxesCurrent[i] = SDL_JoystickGetAxis(mJoystick, i);
    }
    //TODO: Add in bilinear and deadzone reading code!

    for(int i=0;i<getNumButtons();i++)
    {
        mCurrentButtons[i] = SDL_JoystickGetButton(mJoystick, i);

        //check if button was pressed
        if(mCurrentButtons[i] == true && mButtonsPrevious[i] == false)
        {
            mButtonsToggled[i] = !mButtonsToggled[i];
            if(mButtonsToggled[i])
            {
                emit buttonToggled(i);
            }
            emit buttonPressed(i);
        }

        //check if button was released
        else if(mCurrentButtons[i] == false && mButtonsPrevious[i] == true)
        {
            emit buttonReleased(i);
        }

        mButtonsPrevious[i] = mButtonsCurrent[i];
    }
    for(int i=0;i<numHats(); i++)
    {
        mHatsCurrent[i] = SDL_JoystickGetHat(mJoystick, i);
        
        //if hat moved and NO neutral
        if(mHatsCurrent[i] != mHatsPrevious[i] && mHatsCurrent[i] != 0)
        {
            mHatsToggled[i] = !mHatsToggled[i];
            if(mHatsToggled[i])
            {
                emit hatToggled(i);
            }
            emit hatPressed(i);
        }
        
        //if hat moved and IS neutral
        else if(mHatsCurrent[i] != mHatsPrevious && mHatsCurrent[i] == 0)
        {
            emit hatReleased(i);
        }

        mHatsPrevious[i] = mHatsCurrent[i];
    }

    emit axesUpdated(mAxesCurrent);
}
