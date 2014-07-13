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
        QJoystickInterface();
        virtual ~QJoystickInterface();

        /*!
         * These are the directions that can be selected by the hats
         */
        enum HatDirection { Center, Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp };

        public slots:
        /*!
         * Set the current joystick.
         * This will select the joystick with ID
         * jsNumber as the current joystick.
         */
        bool currentJoystick(int id);

        /*!
         * Get the current joystick ID.
         */
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

        /*!
         * Get the name of the current joystick.
         */
        virtual QString name() const = 0;

        /*!
         * Get the number of available joysticks
         */
        virtual int numJoysticks() const = 0;

        /*!
         * Get the number of axes of the current
         * joystick.
         */
        virtual int numAxes() const = 0;

        /*!
         * Get the number of hats of the current
         * joystick
         */
        virtual int numHats() const = 0;

        /*!
         * Get the number of buttons of the current
         * joystick
         */
        virtual int numButtons() const = 0;

        /*!
         * Enable or disable bilinear axis
         * readings
         */
        void bilinearEnable(bool enable);

        /*!
         * Check if bilinear axis reading
         * is enabled or disabled
         */
        bool bilinearEnabled() const;

        /*!
         * Set the bilinear reading
         * adjustment constant.
         */
        void bilinearConstant(double k);

        /*!
         * Get the bilinear reading
         * adjustment constant.
         */
        double bilinearConstant() const;

        /*!
         * Set the deadzone for axis number
         * axisId to size.
         */
        bool deadzone(int axisId, int size);

        /*!
         * Get the deadzone currently set
         * for axis axisId.
         */
        int deadzone(int axisId) const;

        /*!
         * Get the state of the axes.
         */
        const QList<int>& axisStates() const;

        /*!
         * Get the state of the buttons.
         */
        const QList<bool>& buttonStates() const;

        /*!
         * Get the state of the hats.
         */
        QList<int> hatStates();

        bool joystickAttached() const;

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
        void buttonToggled(int button, bool state);

        /*!
         * Signal emitted when hat is pressed.
         * Pressed is defined as off->on
         */
        void hatPressed(int hat, int dir);

        /*!
         * Signal emitted when hat is released.
         * Released is defined as on->off
         */
        void hatReleased(int hat, int dir);

        /*!
         * Signal emitted when hat is toggled.
         * Toggled is defined as going from off->on->off
         */
        void hatToggled(int hat, int dir, bool state);

        /*!
         * Signal emitted on a timer.
         * A timer is used because the axes are analog,
         * and this API shouldn't flood the event processing
         * loop with axesUpdated signals
         */
        void axesUpdated(QList<int> values);

    protected:
        /*!
         * Initialize the joystick data model
         */
        virtual void init();

        /*!
         * Filter the axes data based on the
         * deadzone settings
         */
        void processDeadzones();

        /*!
         * Filter the axes data based on the
         * bilinear reading settings
         */
        void processBilinear();

        /*!
         * Enables/Disables the polling signal
         */
        void enablePolling(bool enable);

        /*!
         * Data structures for holding the current joystick state
         */
        QList<bool> mButtonsCurrent;
        QList<QList< bool> > mHatsCurrent;
        QList<int> mAxesCurrent;

        /*!
         * Data structures for holding the previous joystick state
         */
        QList<bool> mButtonsPrevious;
        QList<QList<bool> > mHatsPrevious;

        /*!
         * Data structures for holding the current joystick toggled state
         */
        QList<bool> mButtonsToggled;
        QList<QList<bool> > mHatsToggled;

    private slots:
        /*!
         * Read the joystick data and store it in
         * the data model
         */
        virtual void getData() = 0;

    private:
        /*!
         * This is the ID of the currently selected joystick
         */
        int mCurrentId;

        /*!
         * These are the bilinear reading settings
         */
        bool mBilinearEnabled;
        double mBilinearConstant;

        /*!
         * These are the deadzones for the axes
         */
        QList<int> mDeadzones;

        /*!
         * This is a timer for polling the physical joystick for data
         */
        QTimer mTimer;
};

#endif // QJOYSTICK_H
