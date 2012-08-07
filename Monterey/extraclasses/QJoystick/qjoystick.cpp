#include "qjoystick.h"

#include <QDebug>

#define POLL_INTERVAL 40

QJoystick::QJoystick()
{
    // Sure, we're only using the Joystick, but SDL doesn't work if video isn't initialised
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
}

int QJoystick::currentJoystick()
{
    return SDL_JoystickIndex(m_joystick);
}

QString QJoystick::joystickName(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return QString(SDL_JoystickName(js));
}

int QJoystick::joystickNumAxes(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumAxes(m_joystick));
}

int QJoystick::joystickNumButtons(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumButtons(m_joystick));
}

int QJoystick::joystickNumHats(int js)  //Added by Chris Konstad
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    return (SDL_JoystickNumHats(m_joystick));
}

void QJoystick::setJoystick(int js)
{
    Q_ASSERT(js < availableJoysticks());
    Q_ASSERT(js >= 0);
    buttonsPrevious.clear();    //konstad
    buttonsToggled.clear(); //konstad
    hatsPrevious.clear();   //Konstad
    hatsToggled.clear();    //konstad
    buttons.clear();    //konstad
    hats.clear();   //konstad

    //SDL_JoystickClose(m_joystick);    //changed by Konstad to possibly fix crashing issue
    m_joystick = SDL_JoystickOpen(js);

    for(int i=0;i<SDL_JoystickNumButtons(m_joystick);i++)   //for each button (konstad)
    {
        buttonsPrevious.append(false);  //start with all set to false
        buttonsToggled.append(false);   //start with all set to false
        buttons.append(false);
    }
    for(int i=0;i<SDL_JoystickNumHats(m_joystick);i++)
    {
        hatsPrevious.append(0);
        hatsToggled.append(false);
        hats.append(0);
    }
    for(int i=0;i<SDL_JoystickNumAxes(m_joystick);i++)
    {
        axis.append(0);
    }
}

int QJoystick::reenumerateDevices()
{
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);   //Quit the subsystem
    SDL_Init(SDL_INIT_JOYSTICK);    //Re-open the subsystem (also reenumerate devices)

    return SDL_NumJoysticks();  //return the number of attached joysticks
}

QJoystick::~QJoystick()
{
    axis.clear();
    buttons.clear();
    hats.clear();   //konstad
    buttonsPrevious.clear();    //konstad
    buttonsToggled.clear(); //konstad
    hatsPrevious.clear();   //konstad
    hatsToggled.clear();    //konstad
    SDL_JoystickClose(m_joystick);
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    this->deleteLater();
}


int QJoystick::availableJoysticks()
{
     return SDL_NumJoysticks();
}

void QJoystick::getdata()
{
    //axis.clear(); //modified by konstad
    //buttons.clear();  //modified by konstad
    //hats.clear();   //konstad


        SDL_Event event;

     SDL_PollEvent(&event);

    for(int i=0;i<SDL_JoystickNumAxes(m_joystick);i++)
    {
        //axis.append(SDL_JoystickGetAxis(m_joystick,i));   //commented out by Konstad
        axis[i] = SDL_JoystickGetAxis(m_joystick, i);
    }

    for(int i=0;i<SDL_JoystickNumButtons(m_joystick);i++)
    {
        //buttons.append(SDL_JoystickGetButton(m_joystick,i));  //commented out by konstad
        buttons[i] = SDL_JoystickGetButton(m_joystick, i);
        if(buttons[i] == true && buttonsPrevious[i] == false)  //if the button just turned true (Konstad)
        {
            buttonsToggled[i] = !buttonsToggled[i];
            emit toggleStateChanged(i);
        }
        buttonsPrevious[i] = buttons[i]; //set the previous value to the current value
    }
    for(int i=0;i<SDL_JoystickNumHats(m_joystick); i++) //konstad
    {
        //hats.append(SDL_JoystickGetHat(m_joystick,i));
        hats[i] = SDL_JoystickGetHat(m_joystick, i);
        if(hats[i] != hatsPrevious[i] && hats[i] != 0)    //if hat moved and NO neutral
        {
            hatsToggled[i] = !hatsToggled[i];
            qDebug() << "Emitting hatStateChanged";
            emit hatStateChanged(hats[i]);
        }
        hatsPrevious[i] = hats[i];  //set the previous value to the current value
    }
}
