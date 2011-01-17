/**
   @file abstractsensor.h
   @brief Base class for sensors

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Semi Malinen <semi.malinen@nokia.com
   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#ifndef ABSTRACTSENSOR_H
#define ABSTRACTSENSOR_H

#include <QTimer>
#include <QStringList>

#include "nodebase.h"
#include "logging.h"
#include "sfwerror.h"
#include "datarange.h"

class AbstractSensorChannel : public NodeBase {
    Q_OBJECT

    // Note: Q_ENUMS makes enumerator names available for QObject::setProperty()
    //Q_ENUMS(SensorState)

    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(SensorError errorCode READ errorCode)
    Q_PROPERTY(int errorCodeInt READ errorCodeInt)
    Q_PROPERTY(QString errorString READ errorString)

    /** Idenfication string for the sensor channel */
    Q_PROPERTY(QString id READ id)

    /** Type of the sensor channel */
    Q_PROPERTY(QString type READ type)

    //Q_PROPERTY(SensorState state READ state)

    /** Whether sensor is running or not */
    Q_PROPERTY(bool running READ running);

public:
    virtual ~AbstractSensorChannel() {};

    bool isValid() const { return isValid_; }

    SensorError errorCode() const { return errorCode_; }
    int errorCodeInt() const { return static_cast<int>(errorCode_); }
    QString errorString() const { return errorString_; }

    const QString& id() const { return id_; }
    const QString type() const { return metaObject()->className(); }

    //SensorState state() const { qDebug() << __PRETTY_FUNCTION__; return state_; }

    bool running() { return (bool)(cnt_ > 0); }

public Q_SLOTS:
    /**
     * @brief Start data flow.
     *
     * This base class implementation is responsible for reference counting.
     * Each child implementation should call the base class method, and verify
     * the return value.
     *
     * Child class implementation is responsible for starting the sensor
     * data flow depending on output from the base class method.
     *
     * @return <b>Base class:</b> \c True when start action should be taken,
     *         \c False if not (i.e already running).<br/>
     *         <b>Child class:</b> \c True on successfull start or already
     *         running, \c False on error.
     */
    virtual bool start();
    bool start(int sessionId);

    /**
     * @brief Stop data flow.
     *
     * This base class implementation is responsible for reference counting.
     * Each child implementation should call the base class method, and verify
     * the return value.
     *
     * Child class implementation is responsible for stopping the sensor
     * data flow depending on output from the base calss method.
     *
     * @return <b>Base class:</b>\c True when start action should be taken,
     *         \c False if not (i.e. Already stopped, or other listeners present).<br/>
     *         <b>Child class:</b>\c True when succesfull, \c False on error.
     */
    virtual bool stop();
    bool stop(int sessionId);

Q_SIGNALS:
    //void stateChanged(SensorState state);
    void errorSignal(int error);

protected:
    AbstractSensorChannel(const QString& id);

    void setError(SensorError errorCode, const QString& errorString);
    void clearError() { errorCode_ = SNoError; errorString_.clear(); }

    bool writeToClients(const void* source, int size);

    void signalPropertyChanged(const QString& name)
    {
        emit propertyChanged(name);
    }

    /*
    void setState(SensorState state)
    {
        if ( state_ != state )
        {
            state_ = state;
            emit stateChanged( state_ );
        }
    }
    */

    QString             id_;
    QString             name_;
    //SensorState         state_;

    SensorError         errorCode_;
    QString             errorString_;

    bool                isValid_;
    int                 cnt_;

    QList<int>          activeSessions_;

    QList<DataRange>        intervalList_;
};

typedef AbstractSensorChannel* (*SensorChannelFactoryMethod)(const QString& id);

#endif // ABSTRACTSENSOR_H