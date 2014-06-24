#ifndef QJOYSTICKINTERFACE_H
#define QJOYSTICKINTERFACE_H

//Original by Dante Sanchez (dante.a.sanchez@gmail.com)
//Modifications (hat, toggle, re-enumeration) by Chris Konstad (chriskon149@gmail.com)

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>

#define POLL_INTERVAL 40

class QJoystickInterface : public QObject
{
    Q_OBJECT
    public:
        virtual QJoystickInterface();
        virtual ~QJoystickInterface();

        public slots:
        /*!
         * Set the current joystick.
         * This will select the joystick with ID
         * jsNumber as the current joystick.
        */
        void currentJoystick(int id);

        int currentJoystick() const;

        /*!
         * Reenumerates the connected joysticks.
         * This restarts SDL's joystick subsystem
         * which can cause significant lag on some
         * systems.  It is the only way (at this time)
         * to rescan the system for newly attached
         * or detatched devices. It returns the number
         * of detected joysticks.
         */
        virtual int reenumerateDevices() = 0; 

        virtual const QString& name() const = 0;

        virtual int numJoysticks() const = 0;

        virtual int numAxes() const = 0;

        virtual int numHats() const = 0;

        virtual int numButtons() const = 0;

        void bilinearEnable(bool enable);

        bool bilinearEnabled() const;

        void bilinearConstant(double k);

        double binearConstant() const;

        bool deadzone(int axisId, int size);

        int deadzone(int axisId) const;


signals:
        /*!
         * Signal emitted when button is pressed.
         * Pressed is defined as off->on
         */
        void buttonPressed(int button);

        /*!
         * Signal emitted when button is released.
         * Released is defined as on->off
         */
        void buttonReleased(int button);

        /*!
         * Signal emitted when button is toggled.
         * Toggled is defined as going from off->on->off
         */
        void buttonToggled(int button);

        /*!
         * Signal emitted when hat is pressed.
         * Pressed is defined as off->on
         */
        void hatPressed(int hat);

        /*!
         * Signal emitted when hat is released.
         * Released is defined as on->off
         */
        void hatReleased(int hat);

        /*!
         * Signal emitted when hat is toggled.
         * Toggled is defined as going from off->on->off
         */
        void hatToggled(int hat);

        /*!
         * Signal emitted on a timer.
         * A timer is used because the axes are analog,
         * and this API shouldn't flood the event processing
         * loop with axesUpdated signals
         */
        void axesUpdated(QList<int> values);

    protected:
        virtual void init();

    private slots:
        virtual void getData() = 0;

    private:
        int mCurrentId;

        QList<bool> mButtonsCurrent;
        QList<int> mHatsCurrent;
        QList<int> mAxesCurrent;

        QList<bool> mButtonsPrevious;
        QList<int> mHatsPrevious;

        QList<bool> mButtonsToggled;
        QList<bool> mHatsToggled;

        bool mBilinearEnabled;
        double mBilinearConstant;

        QList<int> mDeadzones;

        QTimer mTimer;
};

#endif // QJOYSTICK_H
