#ifndef QJOYSTICK_H
#define QJOYSTICK_H

//Original by Dante Sanchez (dante.a.sanchez@gmail.com)
//Modifications (hat, toggle, re-enumeration) by Chris Konstad (chriskon149@gmail.com)

#include <QObject>
#include <QString>
#include <QList>
#include "../QJoystickInterface/qjoystickinterface.h"

#ifdef Q_OS_LINUX
#include "../../SDL-linux/SDL.h"
#else
#include "../../SDL/SDL.h"
#endif

class QJoystick: public QJoystickInterface
{
    Q_OBJECT
    public:
        virtual QJoystick();
        virtual ~QJoystick();

        public slots:
        virtual int reenumerateDevices(); 

        virtual const QString& name() const;

        virtual int numJoysticks() const;

        virtual int numAxes() const;

        virtual int numHats() const;

        virtual int numButtons() const;

    signals:

    protected:
        virtual void init();

    private slots:
        virtual void getData();

    private:
        SDL_Joystick *mJoystick;
};

#endif // QJOYSTICK_H
