#ifndef QJOYSTICK_H
#define QJOYSTICK_H

//Original by Dante Sanchez (dante.a.sanchez@gmail.com)
//Modifications (hat, toggle, re-enumeration) by Chris Konstad (chriskon149@gmail.com)

#include <QObject>
#include <QString>
#include <QList>

#include "../../SDL/SDL.h"

class QJoystick : public QObject
{
    Q_OBJECT
public:
    QJoystick();
    ~QJoystick();
    int availableJoysticks();
    int currentJoystick();
    QString joystickName(int id);
    int joystickNumAxes(int id);
    int joystickNumButtons(int id);
    QList<int> axis;
    QList<bool> buttons;
    QList<bool> buttonsToggled; //Konstad
    QList<bool> hatsToggled; //Konstad
    void getdata();
    int joystickNumHats(int id); //find out the number of hats (Konstad)
    QList<int> hats;    //POV hat (Konstad)

public slots:
    void setJoystick(int jsNumber);
    int reenumerateDevices();   //(konstad) Rescan for joysticks and return # of joysticks

signals:
    void toggleStateChanged(int button);
    void hatStateChanged(int hat);

private:
    SDL_Joystick* m_joystick;
    QList<bool> buttonsPrevious;    //konstad
    QList<int> hatsPrevious;    //konstad
};

#endif // QJOYSTICK_H
